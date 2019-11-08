#include "parseddocument.h"

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
DocumentInfo::Ptr ParsedDocument::extractInfo(const std::string &source, Parser::AST *ast){
    return DocumentInfo::Ptr(nullptr);
}

}} // namespace lv, el
