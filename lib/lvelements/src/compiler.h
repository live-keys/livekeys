#ifndef LVCOMPILER
#define LVCOMPILER

#include "live/elements/lvelementsglobal.h"
#include "live/utf8.h"
#include "live/fileio.h"
#include "live/elements/languageparser.h"

namespace lv{ namespace el{

class BaseNode;
class ProgramNode;

class CompilerPrivate;
class LV_ELEMENTS_EXPORT Compiler{

public:
    typedef std::shared_ptr<Compiler>       Ptr;
    typedef std::shared_ptr<const Compiler> ConstPtr;

    class LV_ELEMENTS_EXPORT Config{

        friend class Compiler;
        friend class CompilerPrivate;

    public:
        Config(
            bool fileOutput = true,
            const std::string& outputExtension = ".js",
            FileIOInterface* ioInterface = new FileIO()
        );

        bool hasCustomElement();
        void addImplicitType(const std::string& typeName);
        void setBaseComponent(const std::string& name, const std::string& importUri);

    private:
        bool                   m_fileOutput;
        FileIOInterface*       m_fileIO;
        std::string            m_outputExtension;
        std::string            m_elementPath;
        std::string            m_baseComponent;
        std::string            m_baseComponentUri;
        std::list<std::string> m_implicitTypes;
        bool                   m_enableJsImports;
    };

public:
    ~Compiler();

    static Ptr create(const Config& config = Config());

    std::string compileToJs(const std::string& path, const std::string& contents);
    std::string compileToJs(const std::string& path, const std::string& contents, LanguageParser::AST* ast);
    std::string compileToJs(const std::string& path, const std::string& content, BaseNode* node);

    std::vector<BaseNode*> collectProgramExports(const std::string& contents, ProgramNode* node);
    ProgramNode* parseProgramNodes(const std::string& path, LanguageParser::AST* ast);

    const std::string& outputExtension() const;
    const LanguageParser::Ptr& parser() const;

    void configureImplicitType(const std::string& type);

private:
    Compiler(const Config& config);

    DISABLE_COPY(Compiler);

    CompilerPrivate* m_d;
};

}}// namespace lv,  el

#endif // LVCOMPILER
