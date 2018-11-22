#ifndef PALETTEMANAGER_H
#define PALETTEMANAGER_H

#include "lveditorglobal.h"
#include "qobjectdefs.h"
#include <list>
#include "qobject.h"
using namespace std;


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

class LV_EDITOR_EXPORT PaletteManager: public QObject
{

    Q_OBJECT
public:


    PaletteManager();
    void paletteAdded(int sb, int span, int height, QObject* p);

    int drawingOffset(int blockNumber, bool forCursor);
    int positionOffset(int y);
    void setTextEdit(TextEdit *value);

    void setLineHeight(int value);
    /** 0 if not, positive number of lines if yes*/
    int isLineBeforePalette(int blockNumber);
    int isLineAfterPalette(int blockNumber);
    int removePalette(QObject* palette);
    int resizePalette(QObject* palette, int newHeight);
public Q_SLOTS:
    void setDirtyPos(int pos);
    void lineNumberChange();
private:
    int lineHeight;
    TextEdit *textEdit;
    list<PaletteData*> palettes;
    int dirtyPos;
    int prevLineNumber;
    int lineNumber;

    void linesAdded();
    void linesRemoved();


    friend class TextControl;
    friend class LineSurface;
};

}

#endif // PALETTEMANAGER_H
