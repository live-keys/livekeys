#include "linecontrol.h"
#include "qmath.h"
#include "textedit_p.h"

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

LineControl::LineControl(QObject *parent) : QObject(parent), m_totalOffset(0)
{
    if (!parent) m_textEdit = new TextEdit(nullptr, true);
    else m_textEdit = static_cast<TextEdit*>(parent);
}

void LineControl::addCollapse(int pos, int num)
{

    std::vector<LineSection> nested;
    auto upper = std::upper_bound(m_sections.begin(), m_sections.end(), LineSection(pos, 0,0,LineSection::Collapsed), LineSection::compare);
    while (upper != m_sections.end()) {
        if (upper->position + upper->range < pos + num)
        {
            nested.push_back(*upper);
            upper = m_sections.erase(upper);
        } else break;
    }

    // do stuff with nested

    unsigned elementPos = insertIntoSorted(LineSection(pos, num, 1, LineSection::Collapsed));
    calculateVisiblePosition(elementPos);
    if (m_sections[elementPos].visibleRange != m_sections[elementPos].range)
        offsetVisibleAfterIndex(elementPos, m_sections[elementPos].rangeOffset());
    m_totalOffset = m_sections[elementPos].rangeOffset();
}

void LineControl::removeCollapse(int pos)
{
    auto lower = std::lower_bound(m_sections.begin(), m_sections.end(), LineSection(pos, 0, 0, LineSection::Collapsed), LineSection::compare);
    unsigned index = static_cast<unsigned>(lower-m_sections.begin());
    offsetVisibleAfterIndex(index, -m_sections[index].rangeOffset());
    m_totalOffset -= lower->rangeOffset();
    m_sections.erase(lower);
}


void LineControl::addPalette(int pos, int span, QQuickItem *p, int startPos, int endPos)
{   
    LineSection ls;
    ls.position = pos;
    ls.range = span;
    ls.type = LineSection::Palette;
    ls.palette = p;
    ls.visibleRange = qCeil((p->height() > 0 ? p->height() + 10 : 0)*1.0/ m_blockHeight);

    ls.startPos = startPos;
    ls.endPos = endPos;

    unsigned elementPos = insertIntoSorted(ls);
    calculateVisiblePosition(elementPos);

    p->setProperty("x", 20);
    p->setProperty("y", ls.visiblePosition * m_blockHeight + (ls.visibleRange * m_blockHeight - p->height()) / 2 + 6);

    if (m_sections[elementPos].visibleRange != m_sections[elementPos].range)
        offsetVisibleAfterIndex(elementPos, m_sections[elementPos].rangeOffset());

    m_totalOffset += m_sections[elementPos].rangeOffset();

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
        offsetVisibleAfterIndex(index, newVisibleRange - it->visibleRange);
        it->visibleRange = newVisibleRange;
        m_totalOffset += newVisibleRange - it->visibleRange;
    }

    return it->position;
}

int LineControl::removePalette(QQuickItem *p)
{
    auto it = m_sections.begin();
    for (; it != m_sections.end(); ++it)
        if (it->type == LineSection::Palette && it->palette == p) break;

    if (it == m_sections.end()) return -1;

    unsigned index = static_cast<unsigned>(it - m_sections.begin());
    offsetVisibleAfterIndex(index, -m_sections[index].rangeOffset());
    m_totalOffset -= m_sections[index].rangeOffset();

    int result = m_sections[index].position;
    m_sections.erase(it);
    return result;

}

void LineControl::removePalette(LineControl::LineSection ls)
{
    auto lower = std::lower_bound(m_sections.begin(), m_sections.end(), ls, LineControl::LineSection::compare);
    unsigned index = static_cast<unsigned>(lower - m_sections.begin());
    offsetVisibleAfterIndex(index, m_sections[index].rangeOffset());
    m_sections.erase(lower);

    m_totalOffset -= ls.rangeOffset();

    QMetaObject::invokeMethod(ls.palette, "close", Qt::DirectConnection);
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
            offsetVisibleAfterIndex(i, newVisibleRange - m_sections[i].visibleRange);
            m_sections[i].visibleRange = newVisibleRange;
            m_totalOffset += newVisibleRange - m_sections[i].visibleRange;
        }
    }
}

void LineControl::reset()
{
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

    for (unsigned i = lower-m_sections.end(); i != m_sections.size(); ++i) {

        bool toBeRemoved = pos <= m_sections[i].startPos && m_sections[i].startPos <= pos + removed;
        toBeRemoved = toBeRemoved || (pos <= m_sections[i].endPos && m_sections[i].endPos <= pos + removed);
        toBeRemoved = toBeRemoved && (removed > 0);

        if (toBeRemoved) deleted.push_back(m_sections[i]);
        else {
            m_sections[i].startPos += added - removed;
            m_sections[i].endPos += added - removed;
        }
    }

    // do stuff if something was deleted

}

int LineControl::drawingOffset(int blockNumber, bool forCursor)
{
    int offset = 0;
    auto upper = std::upper_bound(m_sections.begin(), m_sections.end(), LineSection(blockNumber, 0, 0, LineSection::Collapsed), LineSection::compare);
    if (upper != m_sections.begin() && upper != m_sections.end())
    {
        auto prev = --upper;

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
    auto upper = std::upper_bound(m_sections.begin(), m_sections.end(),
        LineSection(0,0,visibleBlockClicked, LineSection::Collapsed), LineSection::compareVisible);

    if (upper != m_sections.begin() && !m_sections.empty()) {
        auto prev = std::prev(upper);
        if (visibleBlockClicked >= prev->visiblePosition + prev->visibleRange)
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
    return m_totalOffset;
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

int LineControl::isLineBeforePalette(int blockNumber)
{
    auto upper = std::upper_bound(
        m_sections.begin(), m_sections.end(),
        LineSection(blockNumber, 0,0,LineSection::Collapsed),
        LineSection::compare);
    if (upper != m_sections.end())
    {
        if (blockNumber + 1 == upper->position)
            return upper->range;
    }

    return 0;
}

int LineControl::isLineAfterPalette(int blockNumber)
{
    auto upper = std::upper_bound(
        m_sections.begin(), m_sections.end(),
        LineSection(blockNumber, 0,0,LineSection::Collapsed),
        LineSection::compare);
    if (upper != m_sections.begin() && !m_sections.empty())
    {
        auto prev = std::prev(upper);
        if (blockNumber == prev->position + prev->range)
            return prev->range;
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
    m_lineNumber = m_textEdit->documentHandler()->target()->blockCount();
    if (m_prevLineNumber == m_lineNumber) return;

    if (m_prevLineNumber < m_lineNumber) linesAdded();
    else linesRemoved();
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

void LineControl::offsetVisibleAfterIndex(unsigned index, int offset)
{
    for (unsigned i = index + 1; i < m_sections.size(); ++i)
    {
        m_sections[i].visiblePosition += offset;
        if (m_sections[i].type == LineSection::Palette)
        {
            m_sections[i].palette->setProperty("y", m_sections[i].palette->property("y").toInt() + offset * m_blockHeight);
        }
    }
}

void LineControl::offsetAfterIndex(unsigned index, int offset)
{
    for (unsigned i = index + 1; i < m_sections.size(); ++i)
    {
        m_sections[i].position += offset;
        m_sections[i].visiblePosition += offset;
        if (m_sections[i].type == LineSection::Palette)
        {
            m_sections[i].palette->setProperty("y", m_sections[i].palette->property("y").toInt() + offset * m_blockHeight);
        }
    }
}

void LineControl::linesAdded()
{
    int delta = m_lineNumber - m_prevLineNumber;
    auto upper = std::upper_bound(m_sections.begin(), m_sections.end(), LineSection(m_dirtyPos, 0,0,LineSection::Collapsed), LineSection::compare);
    offsetAfterIndex(static_cast<unsigned>(upper-m_sections.begin()-1), delta);
}

void LineControl::linesRemoved()
{
    int delta = m_lineNumber - m_prevLineNumber;
    auto upper = std::upper_bound(m_sections.begin(), m_sections.end(), LineSection(m_dirtyPos, 0,0,LineSection::Collapsed), LineSection::compare);
    offsetAfterIndex(static_cast<unsigned>(upper-m_sections.begin()-1), delta);
}

void LineControl::deltaLines(int delta)
{
    if (delta == 0) return;
    m_prevLineNumber = m_lineNumber;
    m_lineNumber += delta;

    if (m_prevLineNumber < m_lineNumber) linesAdded();
    else linesRemoved();
}

int LineControl::LineSection::rangeOffset()
{
    return visibleRange - range;
}

int LineControl::LineSection::positionOffset()
{
    return visiblePosition - position;
}


} // namespace
