#include "linecontrol.h"
#include "qmath.h"
#include "textedit_p.h"
#include <QQmlProperty>
#include <queue>

namespace lv {

std::function<bool(LineControl::LineSection, LineControl::LineSection)>
    LineControl::LineSection::compare = [](LineSection a, LineSection b)->bool {
        return a.position < b.position;
    };

std::function<bool(LineControl::LineSection, LineControl::LineSection)>
    LineControl::LineSection::compareVisible = [](LineSection a, LineSection b)->bool {
        return a.visiblePosition < b.visiblePosition;
    };

std::function<bool(LineControl::LineSection, LineControl::LineSection)>
    LineControl::LineSection::compareBounds = [](LineSection a, LineSection b)->bool {
        return a.startPos < b.startPos;
    };

LineControl::LineControl(QObject *parent) :
          QObject(parent)
        , m_blockHeight(0)
        , m_dirtyPos(0)
        , m_prevLineNumber(0)
        , m_lineNumber(0)
        , m_totalOffset(0)
        , m_maxWidth(0)
{
    if (!parent) m_textEdit = new TextEdit(nullptr, true);
    else m_textEdit = static_cast<TextEdit*>(parent);

    QObject::connect(m_textEdit, &TextEdit::lineCountChanged, this, &LineControl::lineNumberChange);
}

int LineControl::addCollapse(int pos, int num)
{
    LineSection ls(pos, num + 1, 1, LineSection::Collapsed);

    if (m_textEdit &&
        m_textEdit->code() &&
        m_textEdit->code()->textDocument()){

        auto doc = m_textEdit->code()->textDocument();

        ls.startPos = doc->findBlockByNumber(pos).position();
        ls.endPos = doc->findBlockByNumber(pos + num).position() + doc->findBlockByNumber(pos + num).length() - 1;
    }

    return addLineSection(ls);
}

int LineControl::removeCollapse(int pos)
{
    return removeLineSection(LineSection(pos, 0, 0, LineSection::Collapsed), false);
}


void LineControl::addPalette(int pos, int span, QQuickItem *p, int startPos, int endPos)
{   
    LineSection ls;
    ls.position = pos;
    ls.range = span;
    ls.type = static_cast<int>(p->height()) == 0 ? LineSection::Fragment : LineSection::Palette;
    ls.palette = p;
    ls.visibleRange = qCeil((p->height() > 0 ? p->height() + 10 : 0)*1.0/ m_blockHeight);

    ls.width = static_cast<int>(p->width());
    ls.startPos = startPos;
    ls.endPos = endPos;

    addLineSection(ls);

    emit refreshAfterPaletteChange(pos+span-1, ls.rangeOffset());
}

int LineControl::resizePaletteHeight(QQuickItem *p)
{
    auto it = m_sections.begin();
    for (; it != m_sections.end(); ++it)
        if (it->type == LineSection::Palette && it->palette == p) break;

    if (it == m_sections.end()) return -1;

    int newVisibleRange = qCeil((p->height() > 0 ? p->height() + 10 : 0)*1.0/ m_blockHeight);
    int delta = newVisibleRange - it->visibleRange;

    if (delta != 0)
    {
        unsigned index = static_cast<unsigned>(it - m_sections.begin());
        offsetAfterIndex(index, delta, false);
        m_totalOffset += delta;
        it->visibleRange = newVisibleRange;
        emit refreshAfterPaletteChange(it->position + it->range - 1, delta);
    }

    return it->position;
}

int LineControl::resizePaletteWidth(QQuickItem *p)
{
    auto it = m_sections.begin();
    for (; it != m_sections.end(); ++it)
        if (it->type == LineSection::Palette && it->palette == p) break;

    if (it == m_sections.end()) return -1;

    it->width = static_cast<int>(p->width());

    auto max = 0;
    for (auto it = m_sections.begin(); it != m_sections.end(); ++it)
    {
        if (it->width > max) max = it->width;
    }

    m_maxWidth = max;
}

int LineControl::removePalette(QQuickItem *p, bool destroy)
{
    unsigned i = 0;
    for (; i < m_sections.size(); ++i)
        if (m_sections[i].type == LineSection::Palette
         && m_sections[i].palette == p)
            break;

    if (i == m_sections.size()) return -1;
    int pos = m_sections[i].position;
    int range = m_sections[i].range;
    int delta = m_sections[i].rangeOffset();


    removeLineSection(m_sections[i], destroy);

    auto max = 0;
    for (auto it = m_sections.begin(); it != m_sections.end(); ++it)
    {
        if (it->width > max) max = it->width;
    }

    m_maxWidth = max;

    emit refreshAfterPaletteChange(pos + range - 1, -delta);

    return pos;
}

void LineControl::setBlockHeight(int bh)
{
    m_blockHeight = bh;

    for (unsigned i = 0; i < m_sections.size(); ++i)
    {
        if (m_sections[i].type == LineSection::Collapsed) continue;

        int newVisibleRange = qCeil((m_sections[i].palette->height() > 0 ? m_sections[i].palette->height() + 10 : 0)*1.0/ m_blockHeight);
        if (newVisibleRange != m_sections[i].visibleRange)
        {
            offsetAfterIndex(i, newVisibleRange - m_sections[i].visibleRange, false);
            m_sections[i].visibleRange = newVisibleRange;
            m_totalOffset += newVisibleRange - m_sections[i].visibleRange;
        }
    }

    /*m_dirtyPos = 0;
    updateLinesInDocuments();*/
}

void LineControl::reset()
{
    m_prevLineNumber = m_lineNumber;
    m_lineNumber = 0;
    m_dirtyPos = -1;
    // linesRemoved();
    m_sections.clear();
}

std::vector<LineControl::LineSection> LineControl::sections()
{
    return m_sections;
}

void LineControl::updateSectionBounds(int pos, int removed, int added)
{
    std::list<LineSection> deleted;
    if (m_sections.empty()) return;

    if (removed > 0)
        handleRemovalOfSections(pos, removed);

    if (added > 0)
        handlePositiveShifting(pos, added);
}

int LineControl::drawingOffset(int blockNumber, bool forCursor)
{
    int offset = 0;
    auto upper = std::upper_bound(m_sections.begin(), m_sections.end(),
        LineSection(blockNumber, 0, 0, LineSection::Collapsed), LineSection::compare);
    if (upper != m_sections.begin() && !m_sections.empty())
    {
        auto prev = std::prev(upper);

        if (prev->type == LineSection::Collapsed && blockNumber == prev->position)
        {
            offset = prev->positionOffset();
        }
        else
        if (blockNumber < prev->position + prev->range && !forCursor)
        {
            offset = -blockNumber - 2;
        } else
        if (blockNumber >= prev->position + prev->range)
        {
            offset = prev->positionOffset() + prev->rangeOffset();
        }
    }
    return offset * m_blockHeight;
}

int LineControl::positionOffset(int y)
{
    int visibleBlockClicked = y / m_blockHeight;
    int resultBlock = visibleBlockClicked;

    LineSection search;
    search.visiblePosition = visibleBlockClicked;

    auto upper = std::upper_bound(m_sections.begin(), m_sections.end(),
        search, LineSection::compareVisible);

    if (upper != m_sections.begin() && !m_sections.empty()) {
        auto prev = std::prev(upper);

        if (upper == m_sections.end() && prev->type == LineSection::Fragment
            && prev->position != 0)
        {
            resultBlock = prev->position - 1;
        }
        else if (visibleBlockClicked >= prev->visiblePosition + prev->visibleRange)
        {
            resultBlock = visibleBlockClicked - prev->positionOffset() - prev->rangeOffset();
        }
        else if (prev->type == LineSection::Collapsed && visibleBlockClicked == prev->visiblePosition)
        {
            resultBlock = prev->position;
        } else
        {
            resultBlock = prev->position - 1;
        }
    }

    return resultBlock * m_blockHeight + m_blockHeight / 2;
}

int LineControl::totalOffset()
{
    if (m_sections.empty()) return 0;
    auto lastSection = m_sections[m_sections.size()-1];
    return lastSection.positionOffset() + lastSection.rangeOffset();
}

int LineControl::visibleToAbsolute(int visible)
{
    LineSection ls;
    ls.visiblePosition = visible;
    auto upper = std::upper_bound(m_sections.begin(), m_sections.end(), ls, LineSection::compareVisible);

    int abs = visible;
    if (upper != m_sections.begin())
    {
        auto prev = std::prev(upper);
        if (visible >= prev->visiblePosition + prev->visibleRange)
            abs = visible - prev->positionOffset() - prev->rangeOffset();
        else if (prev->type == LineSection::Collapsed && visible == prev->visiblePosition)
            abs = prev->position;
        else abs = -1; // not mapping to an absolute line
    }

    if (abs >= m_textEdit->code()->textDocument()->blockCount()) return -1;
    return abs;
}

bool LineControl::hiddenByPalette(int blockNumber)
{
    auto upper = std::upper_bound(m_sections.begin(), m_sections.end(), LineSection(blockNumber, 0, 0, LineSection::Palette), LineSection::compare);
    if (upper != m_sections.begin() && !m_sections.empty())
    {
        auto prev = std::prev(upper);
        if (prev->type == LineSection::Palette
                && blockNumber < prev->position + prev->range)
            return true;
    }
    return false;
}

bool LineControl::hiddenByCollapse(int blockNumber)
{
    auto upper = std::upper_bound(m_sections.begin(), m_sections.end(), LineSection(blockNumber, 0, 0, LineSection::Palette), LineSection::compare);
    if (upper != m_sections.begin() && !m_sections.empty())
    {
        auto prev = std::prev(upper);
        if (prev->type == LineSection::Collapsed
                && blockNumber < prev->position + prev->range
                && blockNumber != prev->position)
            return true;
    }
    return false;
}

int LineControl::isJumpForwardLine(int blockNumber, bool forCollapse)
{
    auto lower = std::lower_bound(
        m_sections.begin(), m_sections.end(),
        LineSection(blockNumber, 0,0,LineSection::Collapsed),
        LineSection::compare);
    if (lower != m_sections.end())
    {
        switch (lower->type)
        {
        case LineSection::Palette:
            if (blockNumber + 1 == lower->position && !forCollapse)
                return lower->range;
            break;
        case LineSection::Collapsed:
            if (blockNumber == lower->position)
                return lower->range - 1;
            break;
        case LineSection::Fragment:
            if (blockNumber + 1 == lower->position && !forCollapse)
                return -1;
            break;
        }
    }

    return 0;
}

int LineControl::isJumpBackwardsLine(int blockNumber)
{
    auto upper = std::upper_bound(
        m_sections.begin(), m_sections.end(),
        LineSection(blockNumber, 0,0,LineSection::Collapsed),
        LineSection::compare);
    if (upper != m_sections.begin() && !m_sections.empty())
    {
        auto prev = std::prev(upper);
        if (blockNumber == prev->position + prev->range)
        switch (prev->type)
        {
        case LineSection::Palette:      return prev->range;
        case LineSection::Collapsed:    return prev->range - 1;
        case LineSection::Fragment:     return -1;
        }
    }

    return 0;
}

void LineControl::setDirtyPos(int dirtyPos)
{
    m_dirtyPos = dirtyPos;
}

void LineControl::lineNumberChange()
{
    m_prevLineNumber = m_lineNumber;
    m_lineNumber = m_textEdit->code()->textDocument()->blockCount();
    if (m_prevLineNumber == m_lineNumber) return;

    int delta = m_lineNumber - m_prevLineNumber;
    bool internal = false;

    handleLineChange(delta, internal);

    emit lineDelta(delta, m_dirtyPos, internal);

}

unsigned LineControl::insertIntoSorted(LineControl::LineSection ls)
{
    auto upper = std::upper_bound(m_sections.begin(), m_sections.end(), ls, LineSection::compare);
    unsigned position = static_cast<unsigned>(upper - m_sections.begin());
    m_sections.insert(upper, ls);
    return position;
}

void LineControl::calculateVisiblePosition(unsigned pos)
{
    int off = 0;
    for (unsigned i = 0; i < pos; ++i)
        off += m_sections[i].rangeOffset();
    m_sections[pos].visiblePosition = m_sections[pos].position + off;
}

void LineControl::offsetAfterIndex(unsigned index, int offset, bool offsetPositions, bool offsetVisible)
{
    std::queue<LineSection*> q;
    for (unsigned i = index + 1; i < m_sections.size(); ++i)
    {
        q.push(&m_sections[i]);
        while (!q.empty())
        {
            LineSection* lsp = q.front(); q.pop();
            for (int x = 0; x < lsp->nested.size(); ++x)
                q.push(&lsp->nested[x]);

            if (offsetVisible) lsp->visiblePosition += offset;
            if (offsetPositions) lsp->position += offset;
            if (lsp->type == LineSection::Palette)
            {
                lsp->palette->setY(lsp->visiblePosition * m_blockHeight + (lsp->visibleRange * m_blockHeight - lsp->palette->height()) / 2 + 6);
            }
            else if (lsp->type == LineSection::Fragment) // handles end fragment bounds
            {
                lsp->range = m_lineNumber - lsp->position;
            }
        }
    }
}

void LineControl::handleRemovalOfSections(int pos, int removed)
{
    LineSection search;
    search.startPos = pos + removed;
    auto safe = std::upper_bound(m_sections.begin(), m_sections.end(), search, LineControl::LineSection::compareBounds);

    search.startPos = pos;
    auto iter = std::upper_bound(m_sections.begin(), m_sections.end(), search, LineControl::LineSection::compareBounds);

    bool internal = false;
    if (iter != m_sections.begin())
    {
        iter = std::prev(iter);

        if (iter->startPos <= pos && pos + removed <= iter->endPos)
        {
            internal = true;
            // handling the case when removed text is inside a section (isn't reflected in the visible part of text)

            LineSection* parent = &(*iter);
            while (parent)
            {
                int leftIndex = -1, rightIndex = -1;
                LineSection *newParent = nullptr;
                while (true)
                {
                    for (int i = 0; i < parent->nested.size(); ++i)
                    {
                        if (pos >= parent->nested[i].startPos && pos < parent->nested[i].endPos)
                            leftIndex = i;
                        if (pos + removed >= parent->nested[i].startPos && pos + removed < parent->nested[i].endPos)
                            rightIndex = i;
                    }

                    if (leftIndex == -1 && rightIndex == -1) break;
                    if (leftIndex == rightIndex)
                    {
                        newParent = &parent->nested[leftIndex];
                        break;
                    }

                    if (leftIndex != -1)
                        for (auto sec: parent->nested[leftIndex].nested)
                            parent->nested.push_back(sec);
                    if (rightIndex != -1)
                        for (auto sec: parent->nested[rightIndex].nested)
                            parent->nested.push_back(sec);

                    int offset = 0;
                    if (leftIndex != -1)
                    {
                        parent->nested.erase(parent->nested.begin() + leftIndex);
                        offset = -1;
                    }
                    if (rightIndex != -1)
                        parent->nested.erase(parent->nested.begin() + rightIndex + offset);



                    leftIndex = -1;
                    rightIndex = -1;
                }

                for (unsigned i = 0; i < parent->nested.size(); ++i){
                    if (parent->nested[i].startPos > pos + removed)
                    {
                        parent->nested[i].startPos -= removed;
                        parent->nested[i].endPos -= removed;
                    }
                }

                parent->endPos -= removed;


                if (newParent == nullptr)
                {
                    for (auto it = parent->nested.begin(); it != parent->nested.end();)
                    {
                        if (pos < it->startPos && it->endPos < pos + removed)
                            it = parent->nested.erase(it);
                        else ++it;
                    }
                }



                parent = newParent;


            }
        }
    }

    if (!internal)
    {

        // handle left boundary
        while (true)
        {
            search.startPos = pos;
            auto iter = std::upper_bound(m_sections.begin(), m_sections.end(), search, LineControl::LineSection::compareBounds);
            if (iter != m_sections.begin())
            {
                iter = std::prev(iter);
            }
            else break; // no section that contains left boundary anymore

            if (iter->type == LineSection::Fragment) break;
            if (iter->startPos < pos && pos < iter->endPos)
            {
                if (iter->palette)
                {
                    removePalette(iter->palette);
                } else {
                    std::vector<LineSection> nested = iter->nested;
                    int position = iter->position;
                    int delta = removeCollapse(iter->position);
                    collapseChange(position, delta);
                }
            } else break;
        }

        // handle right boundary
        while (true)
        {
            search.startPos = pos + removed;
            auto iter = std::upper_bound(m_sections.begin(), m_sections.end(), search, LineControl::LineSection::compareBounds);
            if (iter != m_sections.begin())
            {
                iter = std::prev(iter);
            }
            else break; // no section that contains right boundary anymore

            if (iter->type == LineSection::Fragment) break;
            if (iter->startPos < pos + removed && pos + removed < iter->endPos)
            {
                if (iter->palette)
                {
                    removePalette(iter->palette);
                } else {
                    std::vector<LineSection> nested = iter->nested;
                    int pos = iter->position;
                    int delta = removeCollapse(pos);
                    collapseChange(pos, delta);
                }
            } else break;
        }

        // now we should delete all sections between
        while (true)
        {
            search.startPos = pos;
            iter = std::upper_bound(m_sections.begin(), m_sections.end(), search, LineControl::LineSection::compareBounds);

            if (iter != m_sections.end() && iter->startPos < pos + removed)
            {
                if (iter->palette)
                {
                    removePalette(iter->palette);
                } else {
                    std::vector<LineSection> nested = iter->nested;
                    int pos = iter->position;
                    int delta = removeCollapse(pos);
                    collapseChange(pos, delta);
                }

            }
            else break;
        }

        search.startPos = pos + removed;
        safe = std::upper_bound(m_sections.begin(), m_sections.end(), search, LineControl::LineSection::compareBounds);
    }

    for (auto it = safe; it != m_sections.end(); ++it)
    {
        it->startPos -= removed;
        it->endPos -= removed;
    }

}

void LineControl::handlePositiveShifting(int pos, int added)
{
    std::queue<LineSection*> q;

    LineSection search;
    search.startPos = pos;
    auto iter = std::upper_bound(m_sections.begin(), m_sections.end(), search, LineControl::LineSection::compareBounds);

    if (iter != m_sections.begin() && !m_sections.empty())
    {
        iter = std::prev(iter);
    }

    while (iter != m_sections.end())
    {
        q.push(&(*iter));
        ++iter;
    }

    while (!q.empty())
    {
        LineSection* lsp = q.front(); q.pop();
        for (int x = 0; x < lsp->nested.size(); ++x)
            q.push(&lsp->nested[x]);

        if (lsp->startPos <= pos && pos < lsp->endPos)
            lsp->endPos += added;
        if (lsp->startPos > pos)
        {
            lsp->startPos += added;
            lsp->endPos += added;
        }

        if (lsp->type == LineSection::Fragment && lsp->position != 0) // handles end fragment bounds
        {
            lsp->endPos = m_textEdit->code()->textDocument()->characterCount() -1;
        }
    }
}

bool LineControl::handleInternalOffsetting(int index, int delta)
{
    // the lines added/removed are contained inside a single section
    LineSection& sec = m_sections[index];

    std::queue<LineSection*> q;
    q.push(&sec);
    while (!q.empty())
    {
        LineSection* lsp = q.front(); q.pop();
        for (int x = 0; x < lsp->nested.size(); ++x)
            q.push(&lsp->nested[x]);

        if (lsp->position <= m_dirtyPos && m_dirtyPos < lsp->position + lsp->range)
            lsp->range += delta;
        if (lsp->position > m_dirtyPos)
        {
            lsp->position += delta;
            lsp->visiblePosition += delta;
        }

        if (lsp->type == LineSection::Palette)
        {
            lsp->palette->setY(lsp->visiblePosition * m_blockHeight + (lsp->visibleRange * m_blockHeight - lsp->palette->height()) / 2 + 6);
        }
        else if (lsp->type == LineSection::Fragment && lsp->position != 0) // handles end fragment bounds
        {
            lsp->range = m_lineNumber - lsp->position;
        }
    }
    return true;
}

void LineControl::handleLineChange(int delta, bool& internal)
{
    auto upper = std::upper_bound(m_sections.begin(), m_sections.end(), LineSection(m_dirtyPos, 0,0,LineSection::Collapsed), LineSection::compare);

    if (upper != m_sections.begin() && !m_sections.empty())
    {
        auto prev = std::prev(upper);
        if (m_dirtyPos < prev->position + prev->range)
        {
            internal = handleInternalOffsetting(prev-m_sections.begin(), delta);
        }
    }

    offsetAfterIndex(static_cast<unsigned>(upper-m_sections.begin() -1), delta, true, !internal);
}

int LineControl::firstContentLine()
{
    if (m_sections.empty()) return 0;
    auto first = m_sections[0];
    if (first.type == LineSection::Fragment && first.position == 0)
        return first.range;
    return 0;
}

int LineControl::lastContentLine()
{
    int blockCount = m_textEdit->code()->textDocument()->blockCount();
    if (m_sections.empty()) return blockCount;
    auto last = m_sections[m_sections.size()-1];
    if (last.type == LineSection::Fragment && last.position + last.range == blockCount)
        return last.position;
    return blockCount;
}

int LineControl::firstBlockOfTextBefore(int lineNumber)
{
    auto upper = std::upper_bound(m_sections.begin(), m_sections.end(),
        LineSection(lineNumber, 0, 0, LineSection::Collapsed), LineSection::compare);

    if (upper != m_sections.begin())
    {
        auto prev = std::prev(upper);
        int currResult = lineNumber;
        while (true)
        {
            switch (prev->type)
            {
            case LineSection::Collapsed:
                if (currResult > prev->position && currResult < prev->position + prev->range)
                {
                    return prev->position;
                }
                return currResult;
            case LineSection::Palette:
            case LineSection::Fragment:
                if (currResult >= prev->position && currResult < prev->position + prev->range)
                {
                    currResult = prev->position - 1;

                    if (prev == m_sections.begin())
                    {
                        return currResult;
                    }
                    prev = std::prev(prev);
                } else
                {
                    return currResult;
                }
                break;
            }
        }

    }

    return lineNumber;
}

void LineControl::collapseChange(int pos, int delta)
{
    emit refreshAfterCollapseChange(pos, delta);
}

std::vector<VisibleSection> LineControl::visibleSectionsForViewport(const QRect &rect) const
{
    if (m_blockHeight < 1 || rect.width() <0 || rect.height() < 0)
        return std::vector<VisibleSection>();


    int firstBlock = qMax(0, qFloor(rect.y()*1.0/m_blockHeight));
    int lastBlock = qCeil((rect.y() + rect.height())*1.0/m_blockHeight);

    return visibleSections(firstBlock, lastBlock);
}

std::vector<VisibleSection> LineControl::visibleSections(int firstBlock, int lastBlock) const
{    
    std::vector<VisibleSection> result;

    if (m_sections.empty()) {
        result.push_back(VisibleSection(std::min(m_lineNumber - firstBlock, lastBlock - firstBlock + 1), firstBlock));
        return result;
    }

    LineSection ls;
    ls.visiblePosition = firstBlock;

    auto next = std::upper_bound(m_sections.begin(), m_sections.end(), ls, LineSection::compareVisible);
    decltype(next) curr;
    bool noCurr = false;

    if (next != m_sections.begin())
    {
        curr = std::prev(next);
    } else {
        noCurr = true;
    }

    int startPos = firstBlock;
    int total = lastBlock - firstBlock + 1;

    while (total > 0)
    {
        bool aboveCurr = false;
        if (!noCurr)
        {
            switch (curr->type)
            {
            case LineSection::Palette:
                if (startPos < curr->visiblePosition + curr->visibleRange)
                {
                    int size = curr->visibleRange - (startPos - curr->visiblePosition);
                    result.push_back(VisibleSection(std::min(total, size), curr->position, curr->palette));
                    total -= std::min(total, size);
                    startPos = curr->visiblePosition + curr->visibleRange;
                } else {
                    aboveCurr = true;
                }
                break;

            case LineSection::Collapsed:
                if (startPos == curr->visiblePosition)
                {
                    result.push_back(VisibleSection(1, curr->position));
                    total -= 1;
                    startPos = curr->visiblePosition + 1;
                 } else {
                     aboveCurr = true;
                 }
                break;
            case LineSection::Fragment:
                if (curr->position == 0) {
                    aboveCurr = true;
                } else {
                    noCurr = false;
                    total = 0; // the end
                }
                break;
            }
        }

        if (noCurr || aboveCurr)
        {
            if (next == m_sections.end())
            {
                int size = std::min(total, m_lineNumber - startPos + curr->positionOffset() + curr->rangeOffset());
                if (size > 0) result.push_back(VisibleSection(std::min(total, m_lineNumber - startPos + curr->positionOffset() + curr->rangeOffset()), startPos - curr->positionOffset() - curr->rangeOffset()));
                total = 0;
            } else {
                int size = std::min(total, next->visiblePosition - startPos);
                if (size > 0)
                {
                    result.push_back(VisibleSection(std::min(total, next->visiblePosition - startPos), startPos - next->positionOffset()));
                    total -= size;
                }
                startPos = next->visiblePosition;

                curr = next;
                next = std::next(next);
                if (noCurr) noCurr = false;
            }
        }
    }

    return result;
}

void LineControl::deltaLines(int delta)
{
    if (delta == 0) return;
    m_prevLineNumber = m_lineNumber;
    m_lineNumber += delta;

    bool internal = false;

    handleLineChange(delta, internal);
}

int LineControl::maxWidth()
{
    return m_maxWidth;
}

int LineControl::addLineSection(LineControl::LineSection ls)
{
    auto srch = std::upper_bound(m_sections.begin(), m_sections.end(), ls, LineSection::compare);
    int result = 0;
    std::vector<LineSection> nested;
    while (srch != m_sections.end()
           && srch->position + srch->range < ls.position+ls.range)
    {
        nested.push_back(*srch);
        srch++;
    }

    for (LineSection l: nested)
    {
        if (l.type == LineSection::Collapsed)
        {
            result += removeLineSection(l, false, true);
        }
        else {
            l.palette->setVisible(false);
            result += removeLineSection(l, false, true);
        }
    }


    for (LineSection l: ls.nested)
    {
        nested.push_back(l);
    }

    ls.nested = nested;

    unsigned elementPos = insertIntoSorted(ls);
    calculateVisiblePosition(elementPos);

    if (ls.type == LineSection::Palette)
    {
        ls.palette->setProperty("x", 0);
        int yValue = m_sections[elementPos].visiblePosition * m_blockHeight + (m_sections[elementPos].visibleRange * m_blockHeight - static_cast<int>(ls.palette->height())) / 2;
        yValue = yValue > 0 ? yValue : 0;
        ls.palette->setProperty("y", yValue);
    }

    if (ls.width > m_maxWidth) m_maxWidth = ls.width;

    result += m_sections[elementPos].rangeOffset();
    if (m_sections[elementPos].visibleRange != m_sections[elementPos].range)
        offsetAfterIndex(elementPos, m_sections[elementPos].rangeOffset(), false);
    m_totalOffset += m_sections[elementPos].rangeOffset();

    return result;
}

int LineControl::removeLineSection(LineControl::LineSection ls, bool destroy, bool nesting)
{
    auto lower = std::lower_bound(m_sections.begin(), m_sections.end(), ls, LineSection::compare);
    unsigned index = static_cast<unsigned>(lower-m_sections.begin());
    std::vector<LineSection> nested = lower->nested;
    int result = 0;

    if (lower->type == LineSection::Palette && destroy){
        lower->palette->setVisible(false);
        QMetaObject::invokeMethod(lower->palette, "close", Qt::DirectConnection);
    }

    offsetAfterIndex(index, -m_sections[index].rangeOffset(), false);
    m_totalOffset -= lower->rangeOffset();

    result -= lower->rangeOffset();
    m_sections.erase(lower);

    if (!nesting)
    {
        for (LineSection lsx: nested){
            if (lsx.type == LineSection::Palette){
                lsx.palette->setVisible(true);
            }
            result += addLineSection(lsx);

        }
    }

    return result;
}

int LineControl::LineSection::rangeOffset() const
{
    return visibleRange - range;
}

int LineControl::LineSection::positionOffset() const
{
    return visiblePosition - position;
}

VisibleSection::VisibleSection(int sz, int st, QQuickItem* p):
    size(sz), start(st), palette(p) {}


} // namespace
