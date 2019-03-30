#ifndef LINEMANAGER_H
#define LINEMANAGER_H

#include "qtextobject.h"
#include "textedit_p.h"

namespace lv {

class CollapsedSection;
class LineSurface;

/// \private
class LineManager: public QObject{

    Q_OBJECT

public:
    /// \private
    class CollapsedSection {
        public:
            int position;
            int numberOfLines;
            std::list<CollapsedSection*> nestedSections;

            CollapsedSection(int pos = 1, int num = 1);
    };

    LineManager(QObject *parent=nullptr);
    ~LineManager();

    void expandLines(int pos, int num);
    void collapseLines(int pos, int num);
    void linesAdded();
    void linesRemoved();

    std::pair<int, int> isLineAfterCollapsedSection(int m_lineNumber);
    std::pair<int, int> isFirstLineOfCollapsedSection(int m_lineNumber);
    bool isHiddenByCollapse(int pos);
    void setDirtyPos(int p);
    void textDocumentFinishedUpdating(int newLineNumber);
    void setLineDocumentFont(const QFont& font);
    QTextDocument* parentDocument() { return m_parentDocument; }
    void setParentDocument(QTextDocument* td);
    void updateLineVisibility(bool visible, int pos, int num);

private:
    static bool before(int pos1, int num1, int pos2, int num2);
    static bool inside(int pos1, int num1, int pos2, int num2);
    static bool after(int pos1, int num1, int pos2, int num2);
    void writeOutContentOfSections();
    void replaceTextInLineDocumentBlock(int blockNumber, std::string s);
    void changeLastCharInLineDocumentBlock(int blockNumber, char c);
    void updateLinesInDocuments();

    std::list<LineManager::CollapsedSection*> m_sections;
    QTextDocument* m_lineDocument;
    int m_previousLineNumber;
    int m_lineNumber;
    int m_dirtyPos;
    QTextDocument* m_parentDocument;
    bool m_updatePending;

    friend TextDocumentLayout;
};

}

#endif

