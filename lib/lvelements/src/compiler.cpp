#include "compiler.h"
#include "live/visuallog.h"
#include "languagenodes_p.h"
#include "elementssections_p.h"

namespace lv{ namespace el {

class CompilerPrivate{
public:
    CompilerPrivate(const Compiler::Config& pconfig) : config(pconfig){}

    Compiler::Config    config;
    LanguageParser::Ptr parser;

    BaseNode::ConversionContext* createConversionContext(){
        BaseNode::ConversionContext* ctx = new BaseNode::ConversionContext;
        ctx->implicitTypes = config.m_implicitTypes;
        if ( config.hasCustomElement() ){
            ctx->baseComponent = config.m_baseComponent;
            ctx->baseComponentImportUri = config.m_baseComponentUri;
        }
        ctx->jsImportsEnabled = config.m_enableJsImports;

        return ctx;
    }
};

bool Compiler::Config::hasCustomElement(){
    return !m_baseComponent.empty();
}


Compiler::Compiler(const Config &opt)
    : m_d(new CompilerPrivate(opt))
{
    m_d->parser = LanguageParser::createForElements();
}

Compiler::~Compiler(){
    delete m_d;
}

Compiler::Ptr Compiler::create(const Compiler::Config &config){
    return Compiler::Ptr(new Compiler(config));
}

std::string Compiler::compileToJs(const std::string &path, const std::string &contents){
    LanguageParser::AST* ast = m_d->parser->parse(contents);

    std::string result = compileToJs(path, contents, ast);

    m_d->parser->destroy(ast);

    return result;
}

std::string Compiler::compileToJs(const std::string &path, const std::string &contents, LanguageParser::AST *ast){
    std::string result;

    if ( !ast )
        return result;


    ProgramNode* root = parseProgramNodes(path, ast);

    auto ctx = m_d->createConversionContext();
    root->collectImportTypes(contents, ctx);
    delete ctx;

    result = compileToJs(path, contents, root);

    delete root;

    return result;
}

std::string Compiler::compileToJs(const std::string &path, const std::string &contents, BaseNode *node){
    std::string result;
    el::JSSection* section = new el::JSSection;
    section->from = 0;
    section->to   = static_cast<int>(contents.size());

    auto ctx = m_d->createConversionContext();
    node->convertToJs(contents, section->m_children, 0, ctx);
    delete ctx;

    std::vector<std::string> flatten;
    section->flatten(contents, flatten);

    for ( const std::string& s : flatten ){
        result += s;
    }

    delete section;

    if ( m_d->config.m_fileOutput ){
        std::string outputPath = path + m_d->config.m_outputExtension;
        m_d->config.m_fileIO->writeToFile(outputPath, result);
    }

    return result;
}

std::vector<BaseNode *> Compiler::collectProgramExports(const std::string &contents, ProgramNode *node){
    auto ctx = m_d->createConversionContext();
    node->collectImportTypes(contents, ctx);
    delete ctx;

    return node->exports();
}

ProgramNode *Compiler::parseProgramNodes(const std::string &path, LanguageParser::AST *ast){
    if ( !ast )
        return nullptr;

    BaseNode* root = el::BaseNode::visit(ast);

    ProgramNode* pn = dynamic_cast<ProgramNode*>(root);
    if (!path.empty()){
        pn->setFilename(path);
    }

    return pn;
}

const std::string &Compiler::outputExtension() const{
    return m_d->config.m_outputExtension;
}

const LanguageParser::Ptr &Compiler::parser() const{
    return m_d->parser;
}

void Compiler::configureImplicitType(const std::string &type){
    for ( auto it = m_d->config.m_implicitTypes.begin(); it != m_d->config.m_implicitTypes.end(); ++it )
        if ( *it == type )
            return;
    m_d->config.m_implicitTypes.push_back(type);
}

Compiler::Config::Config(bool fileOutput, const std::string &outputExtension, FileIOInterface *ioInterface)
    : m_fileOutput(fileOutput)
    , m_fileIO(ioInterface)
    , m_outputExtension(outputExtension)
    , m_enableJsImports(true)
{
    if ( m_fileOutput && !m_fileIO ){
        THROW_EXCEPTION(lv::Exception, "File reader & writer not defined for compiler.", lv::Exception::toCode("~FileIO"));
    }
    if ( m_fileOutput && m_outputExtension.empty() ){
        THROW_EXCEPTION(lv::Exception, "File extension not defined.", lv::Exception::toCode("~Extensino"));
    }
}

void Compiler::Config::addImplicitType(const std::string &typeName){
    m_implicitTypes.push_back(typeName);
}

void Compiler::Config::setBaseComponent(const std::string &name, const std::string &importUri){
    m_baseComponent = name;
    m_baseComponentUri = importUri;
}

}} // namespace lv, el

