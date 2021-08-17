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
    void reset();

    // LINE BASED FUNCTIONS
    int addCollapse(int pos, int num);
    int removeCollapse(int pos);
    void addPalette(int pos, int span, QQuickItem* p, int startPos, int endPos);
    void resizePaletteHeight(QQuickItem* p);
    void resizePaletteWidth(QQuickItem* p);
    void removePalette(QQuickItem* p, bool destroy = true);

    std::vector<VisibleSection> visibleSections(int firstLine, int lastLine) const;
    void signalRefreshAfterCollapseChange(int pos, int delta);
    int displayLineToSourceLine(int displayLine);

    bool isHiddenByPalette(int lineNumber);
    bool isHiddenByCollapse(int lineNumber);
    bool isFirstLineOfCollapse(int lineNumber);
    int deltaToNextDisplayLine(int lineNumber);
    int deltaToPreviousDisplayLine(int lineNumber);

    int firstContentLine();
    int lastContentLine();
    int firstDisplayLineBefore(int lineNumber);


    // CHARACTER BASED FUNCTIONS
    void contentsChange(int pos, int removed, int added);

    // PIXEL BASED FUNCTIONS
    int transposeClickedPosition(int y);
    std::vector<VisibleSection> visibleSectionsForViewport(const QRect& rect) const;
    int pixelDrawingOffset(int lineNumber);

signals:
    void lineControlCollapsed();
    void lineControlExpand();
    void lineDelta(int delta, int pos, bool internal);
    void refreshAfterCollapseChange(int pos, int delta);
    void refreshAfterPaletteChange(int pos, int delta);
public slots:
    void setFirstDirtyLine(int dirtyLine);
    void lineCountChanged();
    void simulateLineCountChange(int deltaLines);
    int maxWidth();
private:
    int addLineSection(LineSection ls);
    int removeLineSection(LineSection ls, bool destroy, bool restoreNestedPalettes = true);
    unsigned insertIntoSorted(LineSection ls);

    // LINE BASED FUNCTIONS
    void calculateDisplayLine(unsigned sectionIndex);
    void applyOffsetToSectionsAfterIndex(unsigned sectionIndex, int offset, bool offsetPositions, bool offsetVisible = true);
    bool handleOffsetsWithinASection(int sectionIndex, int deltaLines);
    void handleLineCountChanged(int delta, bool& internal);

    // POSITION BASED FUNCTIONS
    void codeRemovalHandler(int pos, int length);
    void codeAddingHandler(int pos, int length);


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
