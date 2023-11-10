/****************************************************************************
**
** Copyright (C) 2022 Dinu SV.
** This file is part of Livekeys Application.
**
** GNU Lesser General Public License Usage
** This file may be used under the terms of the GNU Lesser
** General Public License version 3 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPLv3 included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 3 requirements
** will be met: https://www.gnu.org/licenses/lgpl.html.
**
****************************************************************************/

#include "compiler.h"
#include "live/visuallog.h"
#include "live/packagegraph.h"
#include "live/modulecontext.h"
#include "languagenodes_p.h"
#include "languagenodestojs_p.h"
#include "elementssections_p.h"
#include "elementsmodule.h"
#include "tracepointexception.h"

namespace lv{ namespace el {

class CompilerPrivate{
public:
    CompilerPrivate(const Compiler::Config& pconfig) : config(pconfig), packageGraph(nullptr){}

    Compiler::Config    config;
    LanguageParser::Ptr parser;

    PackageGraph* packageGraph;
    std::map<std::string, ElementsModule::Ptr> loadedModules;
    std::map<std::string, ElementsModule::Ptr> loadedModulesByPath;

    BaseNode::ConversionContext* createConversionContext(
            const Module::Ptr& module = nullptr,
            const std::string& componentPath = "",
            const std::string& relativePathFromBuild = "")
    {
        BaseNode::ConversionContext* ctx = new BaseNode::ConversionContext;
        ctx->implicitTypes = config.m_implicitTypes;
        ctx->outputTypes = config.m_outputTypes;
        if ( config.hasCustomBaseComponent() ){
            ctx->baseComponent = config.m_baseComponent;
            ctx->baseComponentImportUri = config.m_baseComponentUri;
        }
        ctx->allowUnresolved = config.m_allowUnresolved;
        ctx->jsImportsEnabled = config.m_enableJsImports;
        ctx->componentPath = componentPath;
        ctx->relativePathFromBuild = relativePathFromBuild;
        if ( config.componentMetaInfoEnabled() ){
            if ( module && module->context() && !(module->context()->importId.isEmpty()) ){
                ctx->outputComponentMeta = true;
                ctx->currentImportUri = module->context()->importId.data();
            }
        }
        return ctx;
    }
};

bool Compiler::Config::hasCustomBaseComponent(){
    return !m_baseComponent.empty();
}

bool Compiler::Config::componentMetaInfoEnabled(){
    return m_enableComponentMetaInfo;
}

Compiler::Compiler(const Config &opt, PackageGraph* pg)
    : m_d(new CompilerPrivate(opt))
{
    m_d->packageGraph = (pg == nullptr) ? new PackageGraph : pg;
    m_d->parser = LanguageParser::createForElements();
}

Compiler::~Compiler(){
    delete m_d;
}

Compiler::Ptr Compiler::create(const Compiler::Config &config, PackageGraph* pg){
    return Compiler::Ptr(new Compiler(config, pg));
}

FileIOInterface *Compiler::fileIO() const{
    return m_d->config.m_fileIO;
}

const std::list<std::string> &Compiler::importPaths() const{
    return m_d->config.m_importPaths;
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

    std::string name = Path::baseName(path);
    ProgramNode* root = parseProgramNodes(path, name, ast);
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

    LanguageNodesToJs lnt;
    lnt.convert(node, contents, section->m_children, 0, ctx);
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

std::string Compiler::compileModuleFileToJs(const Module::Ptr &module, const std::string &path, const std::string &contents, BaseNode *node){
    std::string result;
    el::JSSection* section = new el::JSSection;
    section->from = 0;
    section->to   = static_cast<int>(contents.size());

    Utf8 outputPath = moduleFileBuildPath(module, path);
    Utf8 relativePathFromOutput;

    std::string pathSeparator(1, Path::separator);
    auto outputPathSegments = outputPath.split(pathSeparator.c_str());
    outputPathSegments.pop_back();
    auto filePathSegments = Utf8(path).split(pathSeparator.c_str());
    filePathSegments.pop_back();

    std::vector<Utf8> relativePathFromOutputSegments;
    size_t i = 0;
    while ( i < outputPathSegments.size() && i < filePathSegments.size() ){
        if ( outputPathSegments[i] != filePathSegments[i] )
            break;
        ++i;
    }
    if ( i == 0 ){
        relativePathFromOutput = Utf8("/") + Utf8::join(outputPathSegments, "/");
    } else {
        for ( size_t j = i; j < outputPathSegments.size(); ++j ){
            if ( !outputPathSegments[j].isEmpty() )
                relativePathFromOutputSegments.push_back("..");
        }
        for ( size_t j = i; j < filePathSegments.size(); ++j ){
            if ( !filePathSegments[j].isEmpty() )
                relativePathFromOutputSegments.push_back(filePathSegments[j]);
        }
    }

    relativePathFromOutput = Utf8::join(relativePathFromOutputSegments, "/");

    auto ctx = m_d->createConversionContext(module, path, relativePathFromOutput.data());
    LanguageNodesToJs lnt;
    lnt.convert(node, contents, section->m_children, 0, ctx);
    delete ctx;

    std::vector<std::string> flatten;
    section->flatten(contents, flatten);

    for ( const std::string& s : flatten ){
        result += s;
    }

    delete section;

    if ( m_d->config.m_fileOutput ){
        std::string outputFile = outputPath.data();
        std::string displayFilePath = path;
        Utf8::replaceAll(displayFilePath, module->packagePath(), "");

        bool shouldWrite = true;
        if ( m_d->config.m_fileOutputOnlyOnModified && Path::exists(outputFile) ){
            DateTime sourceModifiedStamp = Path::lastModified(path);
            DateTime outputModifiedStamp = Path::lastModified(outputFile);
            shouldWrite = outputModifiedStamp < sourceModifiedStamp;
        }
        if ( shouldWrite && module->context() ){
            auto package = module->context()->package;
            if ( !package->release().empty() ){
                shouldWrite = false;
                if ( !Path::exists(outputPath.data()) ){
                    Utf8 msg = Utf8("Released package '%' missing build file: %").format(package->name(), displayFilePath);
                    THROW_EXCEPTION(lv::Exception, msg, Exception::toCode("~File"));
                }
            }
        }

        if ( shouldWrite ){
            m_d->config.m_fileIO->writeToFile(outputPath.data(), result);
            vlog("lvcompiler").v() << "Compiler: Compiled file: " << displayFilePath;
        } else {
            vlog("lvcompiler").v() << "Compiler: Skipped file: " << displayFilePath;
        }
    }

    return result;
}

const std::string &Compiler::packageBuildPath() const{
    return m_d->config.m_packageBuildPath;
}

std::string Compiler::moduleFileBuildPath(const Module::Ptr &plugin, const std::string &path){
    if ( m_d->config.m_packageBuildPath.empty() ){
        return path + m_d->config.m_outputExtension;
    }

    std::string buildPath = createModuleBuildPath(plugin);
    std::string fileName = Path::name(path);
    return Path::join(buildPath, fileName + m_d->config.m_outputExtension);
}

std::string Compiler::moduleBuildPath(const Module::Ptr &module){
    std::string buildDir = Path::join( module->packagePath(), m_d->config.m_packageBuildPath);
    if ( !module->pathFromPackage().empty() ){
        buildDir = Path::join(buildDir, module->pathFromPackage());
    }
    return buildDir;
}


std::string Compiler::createModuleBuildPath(const Module::Ptr &module){
    std::string buildDir = moduleBuildPath(module);
    if ( !Path::exists(buildDir) )
        Path::createDirectories(buildDir);
    return buildDir;
}

std::vector<BaseNode *> Compiler::collectProgramExports(const std::string &contents, ProgramNode *node){
    auto ctx = m_d->createConversionContext();
    node->collectImportTypes(contents, ctx);
    delete ctx;

    return node->exports();
}

ProgramNode *Compiler::parseProgramNodes(const std::string& filePath, const std::string &fileName, LanguageParser::AST *ast){
    if ( !ast )
        return nullptr;
    BaseNode* root = el::BaseNode::visit(filePath, fileName, ast);
    ProgramNode* pn = dynamic_cast<ProgramNode*>(root);
    return pn;
}

const std::string &Compiler::outputExtension() const{
    return m_d->config.m_outputExtension;
}

const std::string &Compiler::importLocalPath() const{
    return m_d->config.m_importLocalPath;
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

std::shared_ptr<ElementsModule> Compiler::compile(Ptr compiler, const std::string &path, Engine *engine){
    std::string modulePath = Path::parent(path);
    std::string fileName = Path::name(path);

    Module::Ptr module(nullptr);
    if ( Module::fileExistsIn(modulePath) ){ // package is now relative to the module
        module = Module::createFromPath(modulePath);
        Package::Ptr package = Package::createFromPath(module->package());
        compiler->m_d->packageGraph->loadRunningPackageAndModule(package, module);
    } else {
        // find package
        std::string packagePath = Module::findPackageFrom(modulePath);
        if ( !packagePath.empty() ){
            // if there's a package, create module normally, it will scan the files and find the package
            module = Module::createFromPath(modulePath);
            Package::Ptr package = Package::createFromPath(module->package());
            compiler->m_d->packageGraph->loadRunningPackageAndModule(package, module);
        } else {
            // if there's no package, create a running module
            module = compiler->m_d->packageGraph->createRunningModule(modulePath);
        }
    }

    auto epl = engine ? ElementsModule::create(module, compiler, engine) : ElementsModule::create(module, compiler);
    ElementsModule::addModuleFile(epl, fileName); // add file if it's not there
    epl->compile();

    return epl;
}

std::shared_ptr<ElementsModule> Compiler::compileModule(Compiler::Ptr compiler, const std::string &path, Engine *engine){
    if ( !Path::exists(path) ){
        THROW_EXCEPTION(lv::Exception, Utf8("Path does not exist: %.").format(path), lv::Exception::toCode("~Path"));
    }
    if ( !Module::existsIn(path) ){
        THROW_EXCEPTION(lv::Exception, Utf8("Module not found in: %.").format(path), lv::Exception::toCode("~Path"));
    }

    Module::Ptr module = Module::createFromPath(path);
    Package::Ptr package = Package::createFromPath(module->package());
    compiler->m_d->packageGraph->loadRunningPackageAndModule(package, module);

    auto epl = engine ? ElementsModule::create(module, compiler, engine) : ElementsModule::create(module, compiler);
    epl->compile();
    return epl;
}

std::vector<std::shared_ptr<ElementsModule> > Compiler::compilePackage(Compiler::Ptr compiler, const std::string &path, Engine *engine){
    if ( !Path::exists(path) ){
        THROW_EXCEPTION(lv::Exception, Utf8("Path doesn't exist: %.").format(path), lv::Exception::toCode("~Path"));
    }
    if ( !Package::existsIn(path) ){
        THROW_EXCEPTION(lv::Exception, Utf8("Package not found in %.").format(path), lv::Exception::toCode("~Path"));
    }

    Package::Ptr package = Package::createFromPath(path);
    auto modules = package->allModules();

    std::vector<std::shared_ptr<ElementsModule> > result;

    for ( auto it = modules.begin(); it != modules.end(); ++it ){
        result.push_back(Compiler::compileModule(compiler, *it, engine));
    }

    return result;
}

std::shared_ptr<ElementsModule> Compiler::compileImportedModule(Compiler::Ptr compiler, const std::string &importKey, const Module::Ptr& requestingModule, Engine *engine){
    auto foundEp = compiler->m_d->loadedModules.find(importKey);
    if ( foundEp == compiler->m_d->loadedModules.end() ){
        try{
            Module::Ptr module = compiler->m_d->packageGraph->loadModule(importKey, requestingModule);
            if ( module  ){
                auto ep = engine ? ElementsModule::create(module , compiler, engine) : ElementsModule::create(module , compiler);
                compiler->m_d->loadedModules[importKey] = ep;
                compiler->m_d->loadedModulesByPath[ep->module()->path()] = ep;
                return ep;
            }
        } catch ( TracePointException& e ){
            throw e;
        } catch ( lv::Exception& e ){
            throw TracePointException(e);
        }
    } else {
        return foundEp->second;
    }
    return nullptr;
}

const std::vector<std::string> &Compiler::packageImportPaths() const{
    return m_d->packageGraph->packageImportPaths();
}

void Compiler::setPackageImportPaths(const std::vector<std::string> &paths){
    m_d->packageGraph->setPackageImportPaths(paths);
}

std::shared_ptr<ElementsModule> Compiler::findLoadedModuleByPath(const std::string &path) const{
    auto it = m_d->loadedModulesByPath.find(path);
    if ( it != m_d->loadedModulesByPath.end() ){
        return it->second;
    }
    return nullptr;
}

Compiler::Config::Config(bool fileOutput, const std::string &outputExtension, FileIOInterface *ioInterface)
    : m_fileOutput(fileOutput)
    , m_fileOutputOnlyOnModified(true)
    , m_fileIO(ioInterface)
    , m_outputExtension(outputExtension)
    , m_enableJsImports(true)
    , m_enableComponentMetaInfo(true)
    , m_allowUnresolved(true)
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

void Compiler::Config::addImportPath(const std::string &path){
    m_importPaths.push_back(path);
}

void Compiler::Config::setBaseComponent(const std::string &name, const std::string &importUri){
    m_baseComponent = name;
    m_baseComponentUri = importUri;
}

void Compiler::Config::initialize(const MLNode &config){
    if ( config.hasKey("baseComponent") ){
        std::string baseComponent = config["baseComponent"].asString();
        std::string baseComponentPath;
        if ( config.hasKey("baseComponentPath") )
            baseComponentPath = config["baseComponentPath"].asString();

        setBaseComponent(baseComponent, baseComponentPath);
    }
    if ( config.hasKey("implicitTypes") ){
        if ( config["implicitTypes"].type() == MLNode::String ){
            Utf8 baseComponent = config["implicitTypes"].asString();
            std::vector<Utf8> split = baseComponent.split(",");
            for ( const Utf8& segm: split ){
                addImplicitType(segm.data());
            }
        } else {
            MLNode::ArrayType a = config["implicitTypes"].asArray();
            for ( const MLNode& n : a ){
                addImplicitType(n.asString());
            }
        }
    }

    if ( config.hasKey("importLocalPath") ){
        m_importLocalPath = config["importLocalPath"].asString();
    }
    if ( config.hasKey("packageBuildPath") ){
        m_packageBuildPath = config["packageBuildPath"].asString();
    }
    if ( config.hasKey("outputExtension") ){
        m_outputExtension = "." + config["outputExtension"].asString();
    }
    if ( config.hasKey("allowUnresolved") ){
        m_allowUnresolved = config["allowUnresolved"].asBool();
    }
    if ( config.hasKey("enableComponentMetaInfo") ){
        m_enableComponentMetaInfo = config["enableComponentMetaInfo"].asBool();
    }
}

}} // namespace lv, el

