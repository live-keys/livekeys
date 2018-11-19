#include "palettemanager.h"
#include <qmath.h>
#include "qdebug.h"
using namespace std;

namespace lv {

PaletteManager::PaletteManager()
{

}

void PaletteManager::paletteAdded(int sb, int span, int height, QObject *p)
{
    auto pd = new PaletteData();
    pd->startBlock = sb;
    pd->lineSpan = span;
    pd->palette = p;
    pd->paletteSpan = qCeil(height*1.0/this->lineHeight);
    pd->palleteHeight = height;



    qDebug() << height << this->lineHeight;


    qDebug() << pd->paletteSpan;

    palettes.push_back(pd);
}

int PaletteManager::drawingOffset(int blockNumber)
{
    auto it = palettes.begin();
    int offset = 0;
    while (it != palettes.end())
    {
        PaletteData* pd = *it;
        if (blockNumber < pd->startBlock) break;

        if (blockNumber < pd->startBlock + pd->lineSpan)
        {
            offset = (-blockNumber - 1)*this->lineHeight;
            break;
        }
        offset += (pd->paletteSpan - pd->lineSpan) * this->lineHeight;

        ++it;
    }

    // qDebug() << blockNumber << offset;
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
}

void PaletteManager::setLineHeight(int value)
{
    lineHeight = value;
}



}
