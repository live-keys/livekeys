#ifndef LVPARSEDDOCUMENT_H
#define LVPARSEDDOCUMENT_H

#include "live/elements/lvelementsglobal.h"
#include "live/elements/languageinfo.h"
#include "live/elements/compiler/languageparser.h"
#include "live/elements/cursorcontext.h"

namespace lv{ namespace el{

class LV_ELEMENTS_EXPORT ParsedDocument{

public:
    static std::vector<ImportInfo> extractImports(const std::string& source, LanguageParser::AST* ast);
    static DocumentInfo::Ptr extractInfo(const std::string& source, LanguageParser::AST* ast);
    static CursorContext findCursorContext(LanguageParser::AST* ast, uint32_t position);
private:
    static void treePath(LanguageParser::AST* ast, int position, std::vector<TSNode>& result);
    static TypeInfo::Ptr extractType(const std::string& source, TSNode node);

    static std::string slice(const std::string& source, TSNode node);
};

}} // namespace lv, el

#endif // LVPARSEDDOCUMENT_H
