#ifndef LINECONTROL_H
#define LINECONTROL_H

#include <QQuickItem>
#include "lveditorglobal.h"

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

        std::vector<LineSection> nested;
    };

    explicit LineControl(QObject *parent = nullptr);
    ~LineControl() {}
    void addCollapse(int pos, int num);
    void removeCollapse(int pos);

    void addPalette(int pos, int span, QQuickItem* p, int startPos, int endPos);
    int resizePalette(QQuickItem* p);
    int removePalette(QQuickItem* p, bool destroy = true);
    void setBlockHeight(int bh);
    int blockHeight() { return m_blockHeight; }
    void reset();
    std::vector<LineSection> sections();
    void updateSectionBounds(int pos, int removed, int added);

    int drawingOffset(int blockNumber, bool forCursor);
    int positionOffset(int y);
    int totalOffset();
    int visibleToAbsolute(int visible);
    int absoluteToVisible(int abs);

    bool hiddenByPalette(int blockNumber);
    bool hiddenByCollapse(int blockNumber);
    int isJumpForwardLine(int blockNumber, bool forCollapse = false);
    int isJumpBackwardsLine(int blockNumber);
    void updateLinesInDocuments();
    QTextDocument* lineDocument();
    void setLineDocumentFont(const QFont& font);

    int firstContentLine();
    int lastContentLine();
    int firstBlockOfTextBefore(int lineNumber);

    std::vector<VisibleSection> visibleSections(int firstLine, int lastLine);
signals:

public slots:
    void setDirtyPos(int dirtyPos);
    void lineNumberChange();
    void deltaLines(int delta);
private:
    void addLineSection(LineSection ls);
    void removeLineSection(LineSection ls, bool destroy, bool nesting = false);
    unsigned insertIntoSorted(LineSection ls);
    void calculateVisiblePosition(unsigned pos);
    void offsetAfterIndex(unsigned index, int offset, bool offsetPositions);
    void linesAdded();
    void linesRemoved();

    void replaceTextInLineDocumentBlock(int blockNumber, std::string s);
    void changeLastCharInLineDocumentBlock(int blockNumber, char c);

    std::vector<LineSection> m_sections;
    int m_blockHeight;

    int m_dirtyPos;
    int m_prevLineNumber;
    int m_lineNumber;
    TextEdit* m_textEdit;
    int m_totalOffset;
    QTextDocument* m_lineDocument;
};

}

#endif // LINECONTROL_H
