#ifndef LANGUAGELVHIGHLIGHTER_H
#define LANGUAGELVHIGHLIGHTER_H

#include <QTextDocument>

#include "editlvsettings.h"
#include "live/documenthandler.h"
#include "live/syntaxhighlighter.h"

#include "live/elements/parser.h"
#include "live/elements/languagequery.h"

namespace lv{

class LanguageLvHighlighter : public SyntaxHighlighter{

public:
    explicit LanguageLvHighlighter(EditLvSettings* settings, DocumentHandler* handler, QTextDocument *parent = nullptr);
    ~LanguageLvHighlighter() override;

    void setTarget(QTextDocument* target);

protected:
    void documentChanged(int, int, int)  override;
    QList<TextFormatRange> highlight(int lastUserState, int position, const QString& text) override;
    QList<TextFormatRange> highlightSections(const QList<ProjectDocumentSection::Ptr>&) override;

private:
    el::Parser             m_parser;
    el::LanguageQuery::Ptr m_languageQuery;
    EditLvSettings*        m_settings;
    el::Parser::AST*       m_currentAst;

    QMap<uint32_t, QTextCharFormat> m_captureToFormatMap;
};


inline void LanguageLvHighlighter::setTarget(QTextDocument *target){
    setDocument(target);
}

}// namespace

#endif // LANGUAGELVHIGHLIGHTER_H
