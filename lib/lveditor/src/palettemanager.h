#ifndef PALETTEMANAGER_H
#define PALETTEMANAGER_H

#include "lveditorglobal.h"
#include <list>

using namespace std;

class QObject;
namespace lv {

class TextEdit;


class LV_EDITOR_EXPORT PaletteData
{
public:
    int startBlock;
    int lineSpan;
    int palleteHeight;
    QObject* palette;
    int paletteSpan;

    bool matchesPalette(QObject* p)
    {
        return palette == p;
    }
};

class LV_EDITOR_EXPORT PaletteManager
{
public:
    PaletteManager();
    void paletteAdded(int sb, int span, int height, QObject* p);

    int drawingOffset(int blockNumber);
    int positionOffset(int y);
    void setTextEdit(TextEdit *value);

    void setLineHeight(int value);

private:
    int lineHeight;
    TextEdit *textEdit;
    list<PaletteData*> palettes;

    friend class TextControl;
};

}

#endif // PALETTEMANAGER_H
