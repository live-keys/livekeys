#ifndef LVPARSER_H
#define LVPARSER_H

#include "live/elements/lvelementsglobal.h"
#include "live/elements/sourcerange.h"

#include <string>
#include <list>
#include <vector>
#include "live/elements/treesitterapi.h"
#include "live/exception.h"

struct TSParser;

namespace lv{ namespace el{

class Engine;

class LV_ELEMENTS_EXPORT SyntaxException: public lv::Exception{
public:
    /** Default contructor */
    SyntaxException(const std::string& message = "", int code = 0) : lv::Exception(message, code){}

    int parsedLine() const;
    int parsedColumn() const;
    int parsedOffset() const;
    std::string parsedFile() const;
    void setParseLocation(int, int, int, const std::string&);
private:
    int m_parsedLine;
    int m_parsedColumn;
    int m_parsedOffset;
    std::string m_parsedFile;
};


class LV_ELEMENTS_EXPORT LanguageParser{

public:
    typedef std::shared_ptr<LanguageParser>       Ptr;
    typedef std::shared_ptr<const LanguageParser> ConstPtr;

public:
    typedef void* AST;
    typedef const void Language;

    class ASTRef{

        friend class lv::el::LanguageParser;

    public:
        ASTRef();
        ASTRef(const ASTRef& other);
        ~ASTRef();

        ASTRef& operator = (const ASTRef& other);

        SourceRange range() const;
        uint32_t childCount() const;
        ASTRef childAt(uint32_t index) const;
        ASTRef parent() const;
        std::string typeString() const;

    private:
        ASTRef(void* node);

        void* m_node;
    };

    class LV_ELEMENTS_EXPORT ComparisonResult{

        friend class LanguageParser;

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
    ~LanguageParser();

    static Ptr create(Language* language);
    static Ptr createForElements();

    AST* parse(const std::string& input) const;
    void editParseTree(LanguageParser::AST*& ast, TSInputEdit& edit, TSInput& input);
    void destroy(AST* ast) const;
    ComparisonResult compare(const std::string& source1, AST* ast1, const std::string& source2, AST* ast2);
    std::string toString(AST* ast) const;

    std::string toJs(const std::string &contents, const std::string filename = "") const;
    std::string toJs(const std::string &contents, AST* ast, const std::string filename = "") const;

    std::list<std::string> parseExportNames(const std::string &moduleFile);
    std::list<std::string> parseExportNames(const std::string& moduleFile, const std::string& content, AST* ast);

    TSParser* internal() const{ return m_parser; }
    Language* language() const;


    Engine *engine() const;
    void setEngine(Engine *engine);

private:
    std::list<std::string> parseExportNamesJs(const std::string& jsModuleFile);

    LanguageParser(Language* language);

    LanguageParser();
    DISABLE_COPY(LanguageParser);

    TSParser* m_parser;
    Language* m_language;
    Engine*   m_engine;
};

}} // namespace lv, el

#endif // LVPARSER_H
