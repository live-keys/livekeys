#ifndef PALETTEMANAGER_H
#define PALETTEMANAGER_H

#include "lveditorglobal.h"
#include "qobjectdefs.h"
#include <list>
#include "qquickitem.h"

namespace lv {

class TextEdit;

/// \private
class PaletteData
{
public:
    int m_startBlock;
    int m_lineSpan;
    int m_paletteHeight;
    QQuickItem* m_palette;
    int m_paletteSpan;
    int m_startPos;
    int m_endPos;

    bool matchesPalette(QQuickItem* p)
    {
        return m_palette == p;
    }
};

/// \private
class PaletteManager: public QObject
{
    Q_OBJECT

public:
    PaletteManager();

    void paletteAdded(int sb, int span, int height, QQuickItem* p, int startPos, int endPos);
    int drawingOffset(int blockNumber, bool forCursor);
    int positionOffset(int y);
    void setTextEdit(TextEdit *value);

    void setLineHeight(int value);
    int isLineBeforePalette(int blockNumber);
    int isLineAfterPalette(int blockNumber);
    int removePalette(QQuickItem* palette);
    int resizePalette(QQuickItem* palette, int newHeight);
    std::list<QQuickItem *> updatePaletteBounds(int pos, int removed, int added);
    std::list<QQuickItem *> deletedOnCollapse(int pos, int num);
    bool isLineUnderPalette(int pos);
    int totalOffset();
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
    int m_totalOffset;

    void linesAdded();
    void linesRemoved();
    void adjustPalettePosition(PaletteData* pd);


    friend class TextControl;
    friend class LineSurface;
};

}

#endif // PALETTEMANAGER_H
