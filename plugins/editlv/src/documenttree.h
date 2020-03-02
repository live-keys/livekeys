#ifndef DOCUMENTTREE_H
#define DOCUMENTTREE_H

#include "live/elements/languageparser.h"
#include "live/projectdocument.h"

namespace lv { namespace el {

class DocumentTree
{
public:
    DocumentTree(ProjectDocument* doc);


    el::LanguageParser::AST *ast() const;

    el::LanguageParser::Ptr parser() const;

    ProjectDocument *document() const;

    void parse();
    void editParseTree(TSInputEdit, TSInput);
    QString slice(SourceRange r);
private:
    el::LanguageParser::Ptr         m_parser;
    el::LanguageParser::AST*        m_ast;
    ProjectDocument*                m_document;
};

} // namespace el
} // namespace lv

#endif // DOCUMENTTREE_H
