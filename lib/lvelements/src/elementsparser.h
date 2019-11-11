#ifndef LVPARSER_H
#define LVPARSER_H

#include "live/elements/lvelementsglobal.h"

#include <string>
#include <list>
#include <vector>
#include "live/elements/treesitterapi.h"

struct TSParser;

namespace lv{ namespace el{

class LV_ELEMENTS_EXPORT Parser{

public:
    typedef void* AST;
    class ASTNodeRef;

    class LV_ELEMENTS_EXPORT ComparisonResult{

        friend class Parser;

    public:
        ComparisonResult(bool isEqual);

        bool isEqual() const{ return m_isEqual; }

        int source1Col() const{ return m_source1Col; }
        int source1Row() const{ return m_source1Row; }
        int source1Offset() const{ return m_source1Offset; }

        int source2Col() const{ return m_source2Col; }
        int source2Row() const{ return m_source2Row; }
        int source2Offset() const{ return m_source2Offset; }

        std::string errorString() const{ return m_errorString; }

    private:
        bool m_isEqual;
        int  m_source1Col;
        int  m_source1Row;
        int  m_source1Offset;
        int  m_source2Col;
        int  m_source2Row;
        int  m_source2Offset;
        std::string m_errorString;
    };

public:
    Parser();
    ~Parser();

    AST* parse(const std::string& input) const;
    void editParseTree(Parser::AST*& ast, TSInputEdit& edit, TSInput& input);
    void destroy(AST* ast) const;
    ComparisonResult compare(const std::string& source1, AST* ast1, const std::string& source2, AST* ast2);
    std::string toString(AST* ast);

    std::string toJs(const std::string &contents, const std::string filename = "") const;
    std::string toJs(const std::string &contents, AST* ast, const std::string filename = "") const;

    static std::list<std::string> parseExportNames(const std::string &moduleFile);

private:
    static std::list<std::string> parseExportNamesJs(const std::string& jsModuleFile);

    DISABLE_COPY(Parser);
    //TODO: Add pointer access

    TSParser* m_parser;
};

}} // namespace lv, el

#endif // LVPARSER_H
