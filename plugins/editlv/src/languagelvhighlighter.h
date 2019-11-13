#ifndef LANGUAGELVHIGHLIGHTER_H
#define LANGUAGELVHIGHLIGHTER_H

#include <QTextDocument>

#include "editlvsettings.h"
#include "live/documenthandler.h"
#include "live/syntaxhighlighter.h"

#include "live/elements/languageparser.h"
#include "live/elements/languagequery.h"

namespace lv{

class LanguageLvHighlighter : public SyntaxHighlighter{

public:
    explicit LanguageLvHighlighter(EditLvSettings* settings, DocumentHandler* handler, QTextDocument *parent = nullptr);
    ~LanguageLvHighlighter() override;

    void setTarget(QTextDocument* target);

    static bool predicateEq(const std::vector<el::LanguageQuery::PredicateData>& args, void* payload);
    static bool predicateEqOr(const std::vector<el::LanguageQuery::PredicateData>& args, void* payload);

protected:
    void documentChanged(int, int, int)  override;
    QList<TextFormatRange> highlight(int lastUserState, int position, const QString& text) override;
    QList<TextFormatRange> highlightSections(const QList<ProjectDocumentSection::Ptr>&) override;

private:
    static QString slice(QTextDocument* doc, int from, int to);

    el::LanguageParser::Ptr  m_parser;
    el::LanguageQuery::Ptr   m_languageQuery;
    EditLvSettings*          m_settings;
    el::LanguageParser::AST* m_currentAst;

    QMap<uint32_t, QTextCharFormat> m_captureToFormatMap;
};


inline void LanguageLvHighlighter::setTarget(QTextDocument *target){
    setDocument(target);
}

}// namespace

#endif // LANGUAGELVHIGHLIGHTER_H
