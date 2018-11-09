#ifndef LVHIGHLIGHTER_H
#define LVHIGHLIGHTER_H

#include "lveditorglobal.h"

#include <QSyntaxHighlighter>

namespace lv{

class DocumentHandler;

class LV_EDITOR_EXPORT Highlighter : public QSyntaxHighlighter{

public:
    Highlighter(DocumentHandler* handler, QObject* parent = 0);
    virtual ~Highlighter();

    DocumentHandler* documentHandler();

protected:
    virtual void highlightBlock(const QString& text);

private:
    DocumentHandler* m_handler;
};

inline DocumentHandler *Highlighter::documentHandler(){
    return m_handler;
}

}// namespace

#endif // LVHIGHLIGHTER_H
