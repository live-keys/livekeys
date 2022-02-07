#ifndef LANGUAGELVHIGHLIGHTER_H
#define LANGUAGELVHIGHLIGHTER_H

#include <QTextDocument>

#include "live/codehandler.h"
#include "live/syntaxhighlighter.h"

#include "live/mlnode.h"

#include "live/elements/compiler/languageparser.h"
#include "live/elements/languagequery.h"

#include "qmllanguageobject.h"
#include "live/textdocumentdata.h"

namespace lv{

class QueryHighlighter : public SyntaxHighlighter{

    Q_OBJECT

public:
    explicit QueryHighlighter(
        QmlLanguageObject* language,
        const MLNode& settings,
        const std::string& pattern,
        CodeHandler* handler,
        QTextDocument* parent);
    ~QueryHighlighter() override;

    static bool predicateEq(const std::vector<el::LanguageQuery::PredicateData>& args, void* payload);
    static const char *parsingCallback(void *payload, uint32_t, TSPoint position, uint32_t *bytes_read);

protected:
    void documentChanged(int, int, int)  override;
    QList<TextFormatRange> highlight(int lastUserState, int position, const QString& text) override;
    QList<TextFormatRange> highlightSections(const QList<ProjectDocumentSection::Ptr>&) override;

private:
    el::LanguageParser::Ptr  m_parser;
    el::LanguageQuery::Ptr   m_languageQuery;
    el::LanguageParser::AST* m_currentAst;
    TextDocumentData*        m_textDocumentData;

    QMap<uint32_t, QTextCharFormat> m_captureToFormatMap;
};

}// namespace

#endif // LANGUAGELVHIGHLIGHTER_H
