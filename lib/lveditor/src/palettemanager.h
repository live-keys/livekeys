#ifndef PALETTEMANAGER_H
#define PALETTEMANAGER_H

#include "lveditorglobal.h"
#include "qobjectdefs.h"
#include <list>
#include "qobject.h"

namespace lv {

class TextEdit;

/// @private
class PaletteData
{
public:
    int m_startBlock;
    int m_lineSpan;
    int m_paletteHeight;
    QObject* m_palette;
    int m_paletteSpan;
    int m_startPos;
    int m_endPos;

    bool matchesPalette(QObject* p)
    {
        return m_palette == p;
    }
};

/// @private
class PaletteManager: public QObject
{
    Q_OBJECT

public:
    PaletteManager();

    void paletteAdded(int sb, int span, int height, QObject* p, int startPos, int endPos);
    int drawingOffset(int blockNumber, bool forCursor);
    int positionOffset(int y);
    void setTextEdit(TextEdit *value);

    void setLineHeight(int value);
    /** 0 if not, positive number of lines if yes*/
    int isLineBeforePalette(int blockNumber);
    int isLineAfterPalette(int blockNumber);
    int removePalette(QObject* palette);
    int resizePalette(QObject* palette, int newHeight);
    std::list<QObject *> updatePaletteBounds(int pos, int removed, int added);

public Q_SLOTS:
    void setDirtyPos(int pos);
    void lineNumberChange();
private:
    int m_lineHeight;
    TextEdit *m_textEdit;
    std::list<PaletteData*> m_palettes;
    int m_dirtyPos;
    int m_prevLineNumber;
    int m_lineNumber;

    void linesAdded();
    void linesRemoved();
    void adjustPalettePosition(PaletteData* pd);


    friend class TextControl;
    friend class LineSurface;
};

}

#endif // PALETTEMANAGER_H
