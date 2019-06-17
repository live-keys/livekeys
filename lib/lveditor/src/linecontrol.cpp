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
{
    if (!parent) m_textEdit = new TextEdit(nullptr, true);
    else m_textEdit = static_cast<TextEdit*>(parent);

    QObject::connect(m_textEdit, &TextEdit::lineCountChanged, this, &LineControl::lineNumberChange);
}

int LineControl::addCollapse(int pos, int num)
{
    LineSection ls(pos, num + 1, 1, LineSection::Collapsed);

    if (m_textEdit &&
        m_textEdit->documentHandler() &&
        m_textEdit->documentHandler()->target()){

        auto doc = m_textEdit->documentHandler()->target();

        ls.startPos = doc->findBlockByNumber(pos).position();
        ls.endPos = doc->findBlockByNumber(pos + num).position() + doc->findBlockByNumber(pos + num).length();

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

    ls.startPos = startPos;
    ls.endPos = endPos;

    addLineSection(ls);
}

int LineControl::resizePalette(QQuickItem *p)
{
    auto it = m_sections.begin();
    for (; it != m_sections.end(); ++it)
        if (it->type == LineSection::Palette && it->palette == p) break;

    if (it == m_sections.end()) return -1;

    int newVisibleRange = qCeil((p->height() > 0 ? p->height() + 10 : 0)*1.0/ m_blockHeight);
    if (newVisibleRange != it->visibleRange)
    {
        unsigned index = static_cast<unsigned>(it - m_sections.begin());
        offsetAfterIndex(index, newVisibleRange - it->visibleRange, false);
        it->visibleRange = newVisibleRange;
        m_totalOffset += newVisibleRange - it->visibleRange;
    }

    return it->position;
}

int LineControl::removePalette(QQuickItem *p, bool destroy)
{
    unsigned i = 0;
    for (; i < m_sections.size(); ++i)
        if (m_sections[i].type == LineSection::Palette
         && m_sections[i].palette == p)
            break;

    if (i == m_sections.size()) return -1;
    int result = m_sections[i].position;

    removeLineSection(m_sections[i], destroy);
    return result;
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
    LineSection search;
    search.startPos = pos;
    auto lower = std::lower_bound(m_sections.begin(), m_sections.end(), search, LineControl::LineSection::compareBounds);

    for (unsigned i = lower-m_sections.begin(); i != m_sections.size(); ++i) {

        bool toBeRemoved = pos <= m_sections[i].startPos && m_sections[i].startPos <= pos + removed;
        toBeRemoved = toBeRemoved || (pos <= m_sections[i].endPos && m_sections[i].endPos <= pos + removed);
        toBeRemoved = toBeRemoved && (removed > 0);

        if (toBeRemoved) deleted.push_back(m_sections[i]);
        else {
            m_sections[i].startPos += added - removed;
            m_sections[i].endPos += added - removed;
        }
    }

    for (LineSection ls: deleted)
        removeLineSection(ls, true);
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

    if (abs >= m_textEdit->documentHandler()->target()->blockCount()) return -1;
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
    //m_prevLineNumber = m_lineNumber;
    m_lineNumber = m_textEdit->documentHandler()->target()->blockCount();
    /*if (m_prevLineNumber == m_lineNumber) return;

    if (m_prevLineNumber < m_lineNumber) linesAdded();
    else linesRemoved();*/
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

void LineControl::offsetAfterIndex(unsigned index, int offset, bool offsetPositions)
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

            lsp->visiblePosition += offset;
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

void LineControl::linesAdded(int newSize)
{
//    auto upper = std::upper_bound(m_sections.begin(), m_sections.end(), LineSection(m_dirtyPos, 0,0,LineSection::Collapsed), LineSection::compare);
//    bool offsetVisible = true;
//    std::queue<LineSection*> q;

//    if (upper != m_sections.begin() && !m_sections.empty())
//    {
//        auto prev = std::prev(upper);
//        if (m_dirtyPos < prev->position + prev->range)
//        {
//            offsetVisible = false;
//            prev->range += delta;
//        }
//    }

//    offsetAfterIndex(static_cast<unsigned>(upper-m_sections.begin()-1), delta, true);
}

//void LineControl::linesRemoved()
//{
//    int delta = m_lineNumber - m_prevLineNumber;
//    auto upper = std::upper_bound(m_sections.begin(), m_sections.end(), LineSection(m_dirtyPos, 0,0,LineSection::Collapsed), LineSection::compare);

//    // handles case of adding lines via palette
//    if (upper != m_sections.begin() && !m_sections.empty())
//    {
//        auto prev = std::prev(upper);
//        if (m_dirtyPos < prev->position + prev->range)
//        {
//            prev->range += delta;
//        }
//    }

//    offsetAfterIndex(static_cast<unsigned>(upper-m_sections.begin()-1), delta, true);

//    for (int i = m_prevLineNumber-1; i >= m_lineNumber; i--)
//    {
//        QTextCursor cursor(m_lineDocument->findBlockByNumber(i));
//        cursor.select(QTextCursor::BlockUnderCursor);
//        cursor.removeSelectedText();
//    }

//    updateLinesInDocuments();
//    m_textEdit->updateLineSurface(m_prevLineNumber, m_lineNumber, m_dirtyPos);
//}

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
    int blockCount = m_textEdit->documentHandler()->target()->blockCount();
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

    /*if (m_prevLineNumber < m_lineNumber) linesAdded();
    else linesRemoved();*/
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
        ls.palette->setProperty("x", 20);
        ls.palette->setProperty("y", m_sections[elementPos].visiblePosition * m_blockHeight + (m_sections[elementPos].visibleRange * m_blockHeight - static_cast<int>(ls.palette->height())) / 2);
    }

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
