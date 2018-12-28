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
    int m_startBlock;
    int m_lineSpan;
    int m_palleteHeight;
    QObject* m_palette;
    int m_paletteSpan;

    bool matchesPalette(QObject* p)
    {
        return m_palette == p;
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
    int m_lineHeight;
    TextEdit *m_textEdit;
    list<PaletteData*> m_palettes;
    int m_dirtyPos;
    int m_prevLineNumber;
    int m_lineNumber;

    void linesAdded();
    void linesRemoved();


    friend class TextControl;
    friend class LineSurface;
};

}

#endif // PALETTEMANAGER_H
