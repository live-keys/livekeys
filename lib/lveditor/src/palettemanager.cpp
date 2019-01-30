#include "palettemanager.h"
#include <qmath.h>
#include "qdebug.h"
#include "textedit_p.h"
using namespace std;

namespace lv {

PaletteManager::PaletteManager()
{
    m_lineNumber = 0;
    m_dirtyPos = -1;
}

bool paletteCmp(PaletteData* a, PaletteData* b)
{
    return a->m_startBlock < b->m_startBlock;
}

void PaletteManager::paletteAdded(int sb, int span, int height, QObject *p)
{
    auto pd = new PaletteData();
    pd->m_startBlock = sb;
    pd->m_lineSpan = span;
    pd->m_palette = p;
    pd->m_paletteHeight = height;
    pd->m_paletteSpan = qCeil((height > 0 ? height + 10 : 0)*1.0/this->m_lineHeight);

    m_palettes.push_back(pd);
    m_palettes.sort(paletteCmp);

    auto it = m_palettes.begin();
    while ((*it)->m_startBlock != pd->m_startBlock) ++it;
    while (it != m_palettes.end()){ adjustPalettePosition((*it)); ++it; }
}

int PaletteManager::drawingOffset(int blockNumber, bool forCursor)
{
    auto it = m_palettes.begin();
    int offset = 0;
    while (it != m_palettes.end())
    {
        PaletteData* pd = *it;
        if (blockNumber < pd->m_startBlock) break;

        if (blockNumber < pd->m_startBlock + pd->m_lineSpan)
        {
            if (forCursor) offset = 0;
            else offset = (-blockNumber - 2)*this->m_lineHeight;
            break;
        }
        offset += (pd->m_paletteSpan - pd->m_lineSpan) * this->m_lineHeight;

        ++it;
    }

    return offset;
}

int PaletteManager::positionOffset(int y)
{
    auto it = m_palettes.begin();
    int offset = y / this->m_lineHeight;
    while (it != m_palettes.end())
    {
        PaletteData* pd = *it;

        if (offset < pd->m_startBlock) break;

        if (offset < pd->m_startBlock + pd->m_paletteSpan)
        {
            offset = pd->m_startBlock - 1;
            break;
        }

        offset += pd->m_lineSpan - pd->m_paletteSpan;
        it++;
    }

    return offset * this->m_lineHeight + this->m_lineHeight / 2;
}

void PaletteManager::setTextEdit(TextEdit *value)
{
    m_textEdit = value;
    if (value != nullptr)
    {
        // QObject::connect(textEdit, &TextEdit::dirtyBlockPosition, this, &PaletteManager::setDirtyPos);
        QObject::connect(m_textEdit, &TextEdit::lineCountChanged, this, &PaletteManager::lineNumberChange);
    }
}

void PaletteManager::setLineHeight(int value)
{
    m_lineHeight = value;
}

int PaletteManager::isLineBeforePalette(int blockNumber)
{
    auto it = m_palettes.begin();
    while (it != m_palettes.end())
    {
        PaletteData* pd = *it;
        if (pd->m_startBlock == blockNumber + 1) return pd->m_lineSpan;
        if (pd->m_startBlock > blockNumber) return 0;

        ++it;
    }

    return 0;
}

int PaletteManager::isLineAfterPalette(int blockNumber)
{
    auto it = m_palettes.begin();
    while (it != m_palettes.end())
    {
        PaletteData* pd = *it;
        if (pd->m_startBlock + pd->m_lineSpan == blockNumber) return pd->m_lineSpan;
        if (pd->m_startBlock >= blockNumber) return 0;

        ++it;
    }

    return 0;
}

int  PaletteManager::removePalette(QObject *palette)
{
    auto it = m_palettes.begin();
    while (it != m_palettes.end())
    {
        PaletteData* pd = *it;
        if (pd->matchesPalette(palette))
        {
            int result = pd->m_startBlock;
            m_palettes.erase(it);
            return result;
        }

        ++it;
    }

    return -1;
}

int PaletteManager::resizePalette(QObject *palette, int newHeight)
{
    auto it = m_palettes.begin();
    while (it != m_palettes.end())
    {
        PaletteData* pd = *it;
        if (pd->matchesPalette(palette))
        {
            pd->m_paletteHeight = newHeight;
            int newPaletteSpan = qCeil(newHeight * 1.0/ this->m_lineHeight);
            if (newPaletteSpan != pd->m_paletteSpan)
            {
                pd->m_paletteSpan = newPaletteSpan;
                return pd->m_startBlock;
            }
            return false;
        }

        ++it;
    }

    return -1;
}

void PaletteManager::setDirtyPos(int pos)
{
    m_dirtyPos = pos;
}

void PaletteManager::lineNumberChange()
{
    m_prevLineNumber = m_lineNumber;
    m_lineNumber = m_textEdit->lineCount();
    if (m_prevLineNumber == m_lineNumber) return;

    if (m_prevLineNumber < m_lineNumber) linesAdded();
    else linesRemoved();
}

void PaletteManager::linesAdded()
{
    int delta = m_lineNumber - m_prevLineNumber;
    auto it = m_palettes.begin();
    while (it != m_palettes.end())
    {
        PaletteData* pd = *it;
        if (m_dirtyPos > pd->m_startBlock + pd->m_lineSpan)
        {
            ++it;
            continue;
        }

        pd->m_startBlock += delta;
        adjustPalettePosition(pd);
        ++it;
    }
}

void PaletteManager::linesRemoved()
{
    int delta = m_prevLineNumber - m_lineNumber;
    auto it = m_palettes.begin();
    while (it != m_palettes.end())
    {
        PaletteData* pd = *it;
        if (m_dirtyPos > pd->m_startBlock + pd->m_lineSpan)
        {
            ++it;
            continue;
        }

        if (m_dirtyPos < pd->m_startBlock && m_dirtyPos + delta >= pd->m_startBlock + pd->m_lineSpan)
        {
            it = m_palettes.erase(it);
            continue;
        }

        pd->m_startBlock -= delta;
        adjustPalettePosition(pd);
        ++it;
    }
}

void PaletteManager::adjustPalettePosition(PaletteData* pd)
{
    auto item = dynamic_cast<QQuickItem*>(pd->m_palette);
    if (!item) return;

    int offset = 0;
    for (auto it = m_palettes.begin(); it != m_palettes.end() && (*it)->m_startBlock < pd->m_startBlock; ++it)
    {
        offset += (*it)->m_paletteSpan-(*it)->m_lineSpan;
    }

    item->setProperty("x", 20);
    item->setProperty("y",(pd->m_startBlock+offset)*this->m_lineHeight + (pd->m_paletteSpan * this->m_lineHeight - pd->m_paletteHeight)/2 + 6);

}



}
