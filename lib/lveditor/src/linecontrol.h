#ifndef LINECONTROL_H
#define LINECONTROL_H

#include <QQuickItem>
#include "lveditorglobal.h"

namespace lv {

class TextEdit;

class LV_EDITOR_EXPORT LineControl : public QObject
{
    Q_OBJECT
public:

    class LV_EDITOR_EXPORT LineSection {
    public:
        enum SectionType { Palette, Collapsed };
        LineSection(int p, int l, int v, SectionType t):
            position(p), range(l), visibleRange(v), type(t), palette(nullptr) {}
        LineSection(): LineSection(0,0,0, Palette) {}

        static std::function<bool(LineSection, LineSection)> compare;
        static std::function<bool(LineSection, LineSection)> compareVisible;
        static std::function<bool(LineSection, LineSection)> compareBounds;
        int rangeOffset();
        int positionOffset();

        int position;
        int range;

        int visiblePosition;
        int visibleRange;

        int startPos;
        int endPos;

        SectionType type;
        QQuickItem* palette;
    };

    explicit LineControl(QObject *parent = nullptr);
    ~LineControl() {}
    void addCollapse(int pos, int num);
    void removeCollapse(int pos);

    void addPalette(int pos, int span, QQuickItem* p, int startPos, int endPos);
    int resizePalette(QQuickItem* p);
    int removePalette(QQuickItem* p);
    void removePalette(LineSection ls);
    void setBlockHeight(int bh);
    void reset();
    std::vector<LineSection> sections();
    void updateSectionBounds(int pos, int removed, int added);

    int drawingOffset(int blockNumber, bool forCursor);
    int positionOffset(int y);
    int totalOffset();

    bool hiddenByPalette(int blockNumber);
    bool hiddenByCollapse(int blockNumber);
    int isLineBeforePalette(int blockNumber);
    int isLineAfterPalette(int blockNumber);
signals:

public slots:
    void setDirtyPos(int dirtyPos);
    void lineNumberChange();
    void deltaLines(int delta);
private:
    unsigned insertIntoSorted(LineSection ls);
    void calculateVisiblePosition(unsigned pos);
    void offsetVisibleAfterIndex(unsigned index, int offset);
    void offsetAfterIndex(unsigned index, int offset);
    void linesAdded();
    void linesRemoved();

    std::vector<LineSection> m_sections;
    int m_blockHeight;

    int m_dirtyPos;
    int m_prevLineNumber;
    int m_lineNumber;
    TextEdit* m_textEdit;
    int m_totalOffset;
};

}

#endif // LINECONTROL_H
