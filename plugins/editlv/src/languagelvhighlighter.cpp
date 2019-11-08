#include "languagelvhighlighter.h"

#include "live/visuallog.h"

namespace lv{

LanguageLvHighlighter::LanguageLvHighlighter(EditLvSettings *settings, DocumentHandler *, QTextDocument *parent)
    : SyntaxHighlighter(parent)
    , m_settings(settings)
{
}

LanguageLvHighlighter::~LanguageLvHighlighter(){
}

QList<SyntaxHighlighter::TextFormatRange> LanguageLvHighlighter::highlight(
        int lastUserState, int position, const QString &text)
{
    return QList<SyntaxHighlighter::TextFormatRange>();
}

QList<SyntaxHighlighter::TextFormatRange> LanguageLvHighlighter::highlightSections(const QList<ProjectDocumentSection::Ptr> &){
    return QList<SyntaxHighlighter::TextFormatRange>();
}

}// namespace
