#ifndef LINECONTROL_H
#define LINECONTROL_H

#include <QQuickItem>
#include "lveditorglobal.h"

#define SCROLL_BUFFER_SIZE 0

class QTextDocument;

namespace lv {

class TextEdit;

class LV_EDITOR_EXPORT VisibleSection {
public:
    VisibleSection(int sz, int st, QQuickItem* p = nullptr);

    int size;
    int start;
    QQuickItem* palette;
};

class LV_EDITOR_EXPORT LineControl : public QObject
{
    Q_OBJECT
public:
    // TODO: make this private
    class LV_EDITOR_EXPORT LineSection {
    public:
        enum SectionType { Palette, Collapsed, Fragment };
        LineSection(int p, int l, int v, SectionType t):
            lineNumber(p), lineSpan(l), displayLineSpan(v), paletteWidth(0), type(t), palette(nullptr) {}
        LineSection(): LineSection(0,0,0, Palette) {}

        static std::function<bool(LineSection, LineSection)> compare;
        static std::function<bool(LineSection, LineSection)> compareVisible;
        static std::function<bool(LineSection, LineSection)> compareBounds;
        int lineSpanDelta() const;
        int lineNumberDelta() const;

        int lineNumber;
        int lineSpan;

        int displayLineNumber;
        int displayLineSpan;

        int startPosition;
        int endPosition;

        int paletteWidth;

        SectionType type;
        QQuickItem* palette;

        std::vector<LineSection> children;
    };

    explicit LineControl(QObject *parent = nullptr);
    ~LineControl() {}

    std::vector<LineSection> sections();
    void setBlockHeight(int bh);
    int blockHeight() { return m_blockHeight; }
    int totalOffset();

    int addCollapse(int pos, int num);
    int removeCollapse(int pos);

    void addPalette(int pos, int span, QQuickItem* p, int startPos, int endPos);
    void resizePaletteHeight(QQuickItem* p);
    void resizePaletteWidth(QQuickItem* p);
    void removePalette(QQuickItem* p, bool destroy = true);

    void reset();
    void contentsChange(int pos, int removed, int added);

    int pixelDrawingOffset(int lineNumber);
    int transposeClickedPosition(int y);
    int displayLineToSourceLine(int visible);
    int absoluteToVisible(int abs);

    bool isHiddenByPalette(int lineNumber);
    bool isHiddenByCollapse(int lineNumber);
    bool isFirstLineOfCollapse(int lineNumber);
    int deltaToNextDisplayLine(int lineNumber);
    int deltaToPreviousDisplayLine(int lineNumber);

    int firstContentLine();
    int lastContentLine();
    int firstDisplayLineBefore(int lineNumber);

    void signalRefreshAfterCollapseChange(int pos, int delta);

    std::vector<VisibleSection> visibleSectionsForViewport(const QRect& rect) const;
    std::vector<VisibleSection> visibleSections(int firstLine, int lastLine) const;
signals:
    void lineControlCollapsed();
    void lineControlExpand();
    void lineDelta(int delta, int pos, bool internal);
    void refreshAfterCollapseChange(int pos, int delta);
    void refreshAfterPaletteChange(int pos, int delta);
public slots:
    void setFirstDirtyLine(int dirtyPos);
    void lineCountChanged();
    void simulateLineCountChange(int delta);
    int maxWidth();
private:
    int addLineSection(LineSection ls);
    int removeLineSection(LineSection ls, bool destroy, bool restoreNestedPalettes = true);
    unsigned insertIntoSorted(LineSection ls);
    void calculateDisplayLine(unsigned pos);
    void applyOffsetToSectionsAfterIndex(unsigned index, int offset, bool offsetPositions, bool offsetVisible = true);
    void codeRemovalHandler(int pos, int removed);
    void codeAddingHandler(int pos, int added);
    bool handleOffsetsWithinASection(int index, int delta);
    void handleLineCountChanged(int delta, bool& internal);

    std::vector<LineSection> m_sections;
    int m_blockHeight;
    int m_firstDirtyLine;
    int m_prevLineNumber;
    int m_lineNumber;
    TextEdit* m_textEdit;
    int m_maxWidth;
};

}

#endif // LINECONTROL_H
