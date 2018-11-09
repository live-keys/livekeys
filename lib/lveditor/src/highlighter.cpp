#include "highlighter.h"

namespace lv{

Highlighter::Highlighter(DocumentHandler *handler, QObject *parent)
    : QSyntaxHighlighter(parent)
    , m_handler(handler)
{
}

Highlighter::~Highlighter(){
}

void Highlighter::highlightBlock(const QString &){
    //TODO: if m_handler->hasSearchHighlights ... )
}

}// namespace
