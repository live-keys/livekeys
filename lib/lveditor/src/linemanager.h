#ifndef LINEMANAGER_H
#define LINEMANAGER_H

#include "qtextobject.h"
#include "textedit_p.h"

namespace lv {

class CollapsedSection;
class LineSurface;

class LV_EDITOR_EXPORT LineManager: public QObject{

    Q_OBJECT

private:
    static bool before(int pos1, int num1, int pos2, int num2);
    static bool inside(int pos1, int num1, int pos2, int num2);
    static bool after(int pos1, int num1, int pos2, int num2);
    void writeOutContentOfSections();
    void replaceTextInLineDocumentBlock(int blockNumber, std::string s);
    void changeLastCharInLineDocumentBlock(int blockNumber, char c);
    void updateLineDocument();
    std::list<CollapsedSection*> sections;
    QTextDocument* lineDocument;
    int previousLineNumber;
    int lineNumber;
    int dirtyPos;
    QTextDocument* parentDocument;
    bool updatePending;

public:
    LineManager(QObject *parent=nullptr);
    ~LineManager();

    Q_INVOKABLE void collapseLines(int pos, int num);
    Q_INVOKABLE void expandLines(int pos, int num);
    void linesAdded();
    void linesRemoved();

    std::list<CollapsedSection*> & getSections();
    std::pair<int, int> isLineAfterCollapsedSection(int lineNumber);
    std::pair<int, int> isFirstLineOfCollapsedSection(int lineNumber);
    void setDirtyPos(int p);
    void textDocumentFinishedUpdating(int newLineNumber);
    void setLineDocumentFont(const QFont& font);
    void setParentDocument(QTextDocument* td);
Q_SIGNALS:
    void updateLineSurface(int prev, int curr, int dirty);
    void showHideTextEditLines(bool visible, int pos, int num);

    friend class TextControl;
    friend class TextEdit;
    friend class TextEditPrivate;
};

}

#endif

