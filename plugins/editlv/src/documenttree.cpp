#include "documenttree.h"
#include <QDebug>

namespace lv { namespace el {

el::LanguageParser::AST *DocumentTree::ast() const
{
    return m_ast;
}

el::LanguageParser::Ptr DocumentTree::parser() const
{
    return m_parser;
}

ProjectDocument *DocumentTree::document() const
{
    return m_document;
}

void DocumentTree::parse()
{

    auto content = m_document->contentString().toStdString();
    m_ast = m_parser->parse(content);
}

void DocumentTree::editParseTree(TSInputEdit edit, TSInput input)
{
    m_parser->editParseTree(m_ast, edit, input);
}

QString DocumentTree::slice(SourceRange r)
{
    return m_document->substring(static_cast<int>(r.from()), static_cast<int>(r.to()-r.from()));
}

lv::el::DocumentTree::DocumentTree(lv::ProjectDocument *doc)
    : m_parser(el::LanguageParser::createForElements())
    , m_ast(nullptr)
    , m_document(doc)
{

}

} // namespace el
} // namespace lv
