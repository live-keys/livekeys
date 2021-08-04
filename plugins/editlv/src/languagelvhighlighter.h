#ifndef LANGUAGELVHIGHLIGHTER_H
#define LANGUAGELVHIGHLIGHTER_H

#include <QTextDocument>

#include "editlvsettings.h"
#include "live/codehandler.h"
#include "live/syntaxhighlighter.h"

#include "live/elements/languageparser.h"
#include "live/elements/languagequery.h"
#include "live/textdocumentdata.h"
#include "documenttree.h"

namespace lv{

class LanguageLvHighlighter : public SyntaxHighlighter{

public:
    explicit LanguageLvHighlighter(EditLvSettings* settings, CodeHandler* handler, el::DocumentTree* tree, QTextDocument *parent = nullptr);
    ~LanguageLvHighlighter() override;

    void setTarget(QTextDocument* target);

    static bool predicateEq(const std::vector<el::LanguageQuery::PredicateData>& args, void* payload);
    static bool predicateEqOr(const std::vector<el::LanguageQuery::PredicateData>& args, void* payload);
    static const char *parsingCallback(void *payload, uint32_t, TSPoint position, uint32_t *bytes_read);
protected:
    void documentChanged(int, int, int)  override;
    QList<TextFormatRange> highlight(int lastUserState, int position, const QString& text) override;
    QList<TextFormatRange> highlightSections(const QList<ProjectDocumentSection::Ptr>&) override;

private:
    static QString slice(QTextDocument* doc, int from, int to);

    el::LanguageQuery::Ptr   m_languageQuery;
    EditLvSettings*          m_settings;
    el::DocumentTree*        m_documentTree;
    TextDocumentData*        m_textDocumentData;

    QMap<uint32_t, QTextCharFormat> m_captureToFormatMap;
};


inline void LanguageLvHighlighter::setTarget(QTextDocument *target){
    setDocument(target);
}

}// namespace

#endif // LANGUAGELVHIGHLIGHTER_H
