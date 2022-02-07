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
#include "languagenodes_p.h"
#include "elementssections_p.h"
#include "elementsmodule.h"

#include <QFileInfo>
#include <QDir>

namespace lv{ namespace el {

class CompilerPrivate{
public:
    CompilerPrivate(const Compiler::Config& pconfig) : config(pconfig), packageGraph(nullptr){}

    Compiler::Config    config;
    LanguageParser::Ptr parser;

    PackageGraph* packageGraph;
    std::map<std::string, ElementsModule::Ptr> loadedModules;
    std::map<std::string, ElementsModule::Ptr> loadedModulesByPath;

    BaseNode::ConversionContext* createConversionContext(){
        BaseNode::ConversionContext* ctx = new BaseNode::ConversionContext;
        ctx->implicitTypes = config.m_implicitTypes;
        if ( config.hasCustomBaseComponent() ){
            ctx->baseComponent = config.m_baseComponent;
            ctx->baseComponentImportUri = config.m_baseComponentUri;
        }
        ctx->jsImportsEnabled = config.m_enableJsImports;

        return ctx;
    }
};

bool Compiler::Config::hasCustomBaseComponent(){
    return !m_baseComponent.empty();
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

std::string Compiler::compileModuleFileToJs(const Module::Ptr &plugin, const std::string &path, const std::string &contents, BaseNode *node){
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
        std::string outputPath = moduleFileBuildPath(plugin, path);
        m_d->config.m_fileIO->writeToFile(outputPath, result);
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
    std::string fileName = QFileInfo(QString::fromStdString(path)).fileName().toStdString();
    return buildPath + "/" + fileName + m_d->config.m_outputExtension;
}

std::string Compiler::moduleBuildPath(const Module::Ptr &module){
    std::string buildDir = module->packagePath() + "/" + m_d->config.m_packageBuildPath;
    if ( !module->pathFromPackage().empty() ){
        buildDir += "/" + module->pathFromPackage();
    }
    return buildDir;
}


std::string Compiler::createModuleBuildPath(const Module::Ptr &module){
    std::string buildDir = moduleBuildPath(module);
    QString bd = QString::fromStdString(buildDir);
    if ( !QDir(bd).exists() ){
        QDir(".").mkpath(bd);
    }

    return buildDir;
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
    QFileInfo finfo(QString::fromStdString(path));
    std::string pluginPath = finfo.path().toStdString();
    std::string fileName = finfo.fileName().toStdString();

    Module::Ptr plugin(nullptr);
    if ( Module::existsIn(pluginPath) ){
        plugin = Module::createFromPath(pluginPath);
        Package::Ptr package = Package::createFromPath(plugin->package());
        compiler->m_d->packageGraph->loadRunningPackageAndModule(package, plugin);
    } else {
        plugin = compiler->m_d->packageGraph->createRunningModule(pluginPath);
    }

    auto epl = engine ? ElementsModule::create(plugin, engine) : ElementsModule::create(plugin, compiler);
    ElementsModule::addModuleFile(epl, fileName);
    epl->compile();

    return epl;
}

std::shared_ptr<ElementsModule> Compiler::compileImport(Compiler::Ptr compiler, const std::string &importKey, const Module::Ptr& requestingModule, Engine *engine){
    auto foundEp = compiler->m_d->loadedModules.find(importKey);
    if ( foundEp == compiler->m_d->loadedModules.end() ){
        Module::Ptr plugin = compiler->m_d->packageGraph->loadModule(importKey, requestingModule);
        if ( plugin ){
            auto ep = engine ? ElementsModule::create(plugin, engine) : ElementsModule::create(plugin, compiler);
            compiler->m_d->loadedModules[importKey] = ep;
            compiler->m_d->loadedModulesByPath[ep->module()->path()] = ep;
            return ep;
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
}

}} // namespace lv, el

