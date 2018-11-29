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

    LineSurface* myLineSurface;
    std::list<CollapsedSection*> sections;

public:
    LineManager(QObject *parent=nullptr);
    ~LineManager();

    Q_INVOKABLE void collapseLines(int pos, int num);
    Q_INVOKABLE void expandLines(int pos, int num);
    void linesAdded(int pos, int num);
    void linesRemoved(int pos, int num);

    std::list<CollapsedSection*> & getSections();
    void setLineSurface(LineSurface* lns);
    std::pair<int, int> isLineAfterCollapsedSection(int lineNumber);
    std::pair<int, int> isFirstLineOfCollapsedSection(int lineNumber);

    friend class TextEdit;
    friend class TextControl;
};

}

#endif

