#include "parseddocument.h"
#include "live/visuallog.h"

#include "tree_sitter/parser.h"
#include "tree_sitter/api.h"

namespace lv{ namespace el{

//TODO
std::vector<ImportInfo> ParsedDocument::extractImports(const std::string &/*source*/, Parser::AST */*ast*/){
    return std::vector<ImportInfo>();
}

//TODO
CursorContext ParsedDocument::findCursorContext(Parser::AST */*ast*/, int /*position*/){
    return CursorContext(0, std::vector<SourceRange>());
}

//TODO
DocumentInfo::Ptr ParsedDocument::extractInfo(const std::string &, Parser::AST *){
    return DocumentInfo::Ptr(nullptr);
}

}} // namespace lv, el
