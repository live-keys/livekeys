#include "palettemanager.h"
#include <qmath.h>
#include "qdebug.h"
#include "textedit_p.h"
using namespace std;

namespace lv {

PaletteManager::PaletteManager()
{
    lineNumber = 0;
    dirtyPos = -1;
}

bool paletteCmp(PaletteData* a, PaletteData* b)
{
    return a->startBlock < b->startBlock;
}

void PaletteManager::paletteAdded(int sb, int span, int height, QObject *p)
{
    auto pd = new PaletteData();
    pd->startBlock = sb;
    pd->lineSpan = span;
    pd->palette = p;
    pd->paletteSpan = qCeil(height*1.0/this->lineHeight);
    pd->palleteHeight = height;

    palettes.push_back(pd);
    palettes.sort(paletteCmp);
}

int PaletteManager::drawingOffset(int blockNumber, bool forCursor)
{
    auto it = palettes.begin();
    int offset = 0;
    while (it != palettes.end())
    {
        PaletteData* pd = *it;
        if (blockNumber < pd->startBlock) break;

        if (blockNumber < pd->startBlock + pd->lineSpan)
        {
            if (forCursor) offset = 0;
            else offset = (-blockNumber - 1)*this->lineHeight;
            break;
        }
        offset += (pd->paletteSpan - pd->lineSpan) * this->lineHeight;

        ++it;
    }

    return offset;
}

int PaletteManager::positionOffset(int y)
{
    auto it = palettes.begin();
    int offset = y / this->lineHeight;
    while (it != palettes.end())
    {
        PaletteData* pd = *it;

        if (offset < pd->startBlock) break;

        if (offset < pd->startBlock + pd->paletteSpan)
        {
            offset = pd->startBlock - 1;
            break;
        }

        offset += pd->lineSpan - pd->paletteSpan;
        it++;
    }

    return offset * this->lineHeight + this->lineHeight / 2;
}

void PaletteManager::setTextEdit(TextEdit *value)
{
    textEdit = value;
    if (value != nullptr)
    {
        QObject::connect(textEdit, &TextEdit::dirtyBlockPosition, this, &PaletteManager::setDirtyPos);
        QObject::connect(textEdit, &TextEdit::lineCountChanged, this, &PaletteManager::lineNumberChange);
    }
}

void PaletteManager::setLineHeight(int value)
{
    lineHeight = value;
}

int PaletteManager::isLineBeforePalette(int blockNumber)
{
    auto it = palettes.begin();
    while (it != palettes.end())
    {
        PaletteData* pd = *it;
        if (pd->startBlock == blockNumber + 1) return pd->lineSpan;
        if (pd->startBlock > blockNumber) return 0;

        ++it;
    }

    return 0;
}

int PaletteManager::isLineAfterPalette(int blockNumber)
{
    auto it = palettes.begin();
    while (it != palettes.end())
    {
        PaletteData* pd = *it;
        if (pd->startBlock + pd->lineSpan == blockNumber) return pd->lineSpan;
        if (pd->startBlock >= blockNumber) return 0;

        ++it;
    }

    return 0;
}

int  PaletteManager::removePalette(QObject *palette)
{
    auto it = palettes.begin();
    while (it != palettes.end())
    {
        PaletteData* pd = *it;
        if (pd->matchesPalette(palette))
        {
            int result = pd->startBlock;
            palettes.erase(it);
            return result;
        }

        ++it;
    }

    return -1;
}

int PaletteManager::resizePalette(QObject *palette, int newHeight)
{
    auto it = palettes.begin();
    while (it != palettes.end())
    {
        PaletteData* pd = *it;
        if (pd->matchesPalette(palette))
        {
            pd->palleteHeight = newHeight;
            int newPaletteSpan = qCeil(newHeight / this->lineHeight);
            if (newPaletteSpan != pd->paletteSpan)
            {
                pd->paletteSpan = newPaletteSpan;
                return pd->startBlock;
            }
            return false;
        }

        ++it;
    }

    return -1;
}

void PaletteManager::setDirtyPos(int pos)
{
    dirtyPos = pos;
}

void PaletteManager::lineNumberChange()
{
    prevLineNumber = lineNumber;
    lineNumber = textEdit->lineCount();
    if (prevLineNumber == lineNumber) return;

    if (prevLineNumber < lineNumber) linesAdded();
    else linesRemoved();
}

void PaletteManager::linesAdded()
{
    int delta = lineNumber - prevLineNumber;
    auto it = palettes.begin();
    while (it != palettes.end())
    {
        PaletteData* pd = *it;
        if (dirtyPos > pd->startBlock + pd->lineSpan)
        {
            ++it;
            continue;
        }

        pd->startBlock += delta;
        ++it;
    }
}

void PaletteManager::linesRemoved()
{
    int delta = prevLineNumber - lineNumber;
    auto it = palettes.begin();
    qDebug() << dirtyPos;
    qDebug() << delta;
    while (it != palettes.end())
    {
        PaletteData* pd = *it;
        qDebug() << pd->startBlock;
        qDebug() << pd->lineSpan;
        if (dirtyPos > pd->startBlock + pd->lineSpan)
        {
            ++it;
            continue;
        }

        if (dirtyPos < pd->startBlock && dirtyPos + delta >= pd->startBlock + pd->lineSpan)
        {
            it = palettes.erase(it);
            continue;
        }

        pd->startBlock -= delta;
        ++it;
    }
}



}
