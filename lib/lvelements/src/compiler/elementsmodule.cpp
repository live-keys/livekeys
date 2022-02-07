#include "elementsmodule.h"
#include "modulefile.h"
#include "live/modulecontext.h"
#include "live/exception.h"
#include "live/fileio.h"
#include <QFile>

#ifdef BUILD_ELEMENTS_ENGINE
#include "live/elements/engine.h"
#include "live/elements/modulelibrary.h"
#endif

namespace lv{ namespace el{

/**
 * \class ElementsModule
 * \brief Container for module functionality on the Elements side.
 */

class ElementsModulePrivate{
public:
    ElementsModulePrivate(Engine* e)
        : engine(e), typesResolved(false), isCompiled(false){}

    Module::Ptr   module;
    Compiler::Ptr compiler;
    Engine*       engine;
    std::map<std::string, ModuleFile*> fileModules;
    std::list<ModuleLibrary*>          libraries;

    std::map<std::string, ElementsModule::Export> exports;

    bool typesResolved;
    bool isCompiled;
};


ElementsModule::ElementsModule(Module::Ptr plugin, Compiler::Ptr compiler, Engine *engine)
    : m_d(new ElementsModulePrivate(engine))
{
    m_d->compiler = compiler;
    m_d->module = plugin;
}


ElementsModule::~ElementsModule(){
#ifdef BUILD_ELEMENTS_ENGINE
    for ( auto it = m_d->libraries.begin(); it != m_d->libraries.end(); ++it ){
        delete *it;
    }
#endif
    delete m_d;
}

#ifdef BUILD_ELEMENTS_ENGINE
ElementsModule::Ptr ElementsModule::create(Module::Ptr module, Engine *engine){
    return create(module, engine->compiler(), engine);
}
#else
ElementsModule::Ptr ElementsModule::create(Module::Ptr, Engine *){
    THROW_EXCEPTION(lv::Exception, "This build does not support the engine as a module loader.", lv::Exception::toCode("~Build"));
}
#endif

ElementsModule::Ptr ElementsModule::create(Module::Ptr module, Compiler::Ptr compiler){
    return create(module, compiler, nullptr);
}

ElementsModule::Ptr ElementsModule::create(Module::Ptr module, Compiler::Ptr compiler, Engine *engine){
    ElementsModule::Ptr epl(new ElementsModule(module, compiler, engine));

    for ( auto it = module->fileModules().begin(); it != module->fileModules().end(); ++it ){
        ElementsModule::addModuleFile(epl, *it + ".lv");
    }

    epl->initializeLibraries(module->libraryModules());

    return epl;
}


void ElementsModule::initializeLibraries(const std::list<std::string> &libs){
#ifdef BUILD_ELEMENTS_ENGINE
    for ( auto it = libs.begin(); it != libs.end(); ++it ){
        ModuleLibrary* lib = ModuleLibrary::load(m_d->engine, *it);
        //TODO: Load library exports (instances and types)
        m_d->libraries.push_back(lib);
    }
#else
    if ( !libs.empty() )
        THROW_EXCEPTION(lv::Exception, "Module contains libraries that cannot be loaded in this build", lv::Exception::toCode("~Build"));
#endif
}

ModuleFile *ElementsModule::addModuleFile(ElementsModule::Ptr &epl, const std::string &name){
    auto it = epl->m_d->fileModules.find(name);
    if ( it != epl->m_d->fileModules.end() ){
        return it->second;
    }

    Compiler::Ptr compiler = epl->m_d->compiler;

    std::string filePath = epl->module()->path() + "/" + name;
    std::string content = compiler->fileIO()->readFromFile(filePath);
    LanguageParser::AST* ast = compiler->parser()->parse(content);

    std::string componentName = name;
    size_t i = componentName.find(".lv");
    if ( i != std::string::npos ){
        componentName = name.substr(0, i);
    }

    ProgramNode* pn = compiler->parseProgramNodes(componentName, ast);

    ModuleFile* mf = new ModuleFile(epl, name, content, pn);
    epl->m_d->fileModules[name] = mf;

    auto mfExports = mf->exports();

    for ( auto it = mfExports.begin(); it != mfExports.end(); ++it ){
        ElementsModule::Export exp;
        exp.name = it->name;
        exp.type = it->type == ModuleFile::Export::Element ? ElementsModule::Export::Element : ElementsModule::Export::Component;
        exp.file = mf;
        epl->m_d->exports.insert(std::make_pair(exp.name, exp));
    }

    auto mfImports = mf->imports();
    for ( auto it = mfImports.begin(); it != mfImports.end(); ++it ){
        ModuleFile::Import& imp = *it;
        if ( imp.isRelative ){
            if ( epl->module()->context()->package == nullptr ){
                THROW_EXCEPTION(lv::Exception, "Cannot import relative path withouth package: " + imp.uri, Exception::toCode("~Import"));
            }

            std::string importUri = imp.uri.substr(1);
            ElementsModule::Ptr ep = Compiler::compileImport(epl->m_d->compiler, importUri, epl->module(), epl->engine());
            if ( !ep ){
                THROW_EXCEPTION(lv::Exception, "Failed to find module: " + imp.uri, Exception::toCode("~Import"));
            }

            mf->resolveImport(imp.uri, ep);

        } else {
            ElementsModule::Ptr ep = Compiler::compileImport(epl->m_d->compiler, it->uri, epl->module(), epl->engine());
            if ( !ep ){
                THROW_EXCEPTION(lv::Exception, "Failed to find module: " + imp.uri, Exception::toCode("~Import"));
            }
            mf->resolveImport(imp.uri, ep);
        }
    }
    return mf;
}

ModuleFile *ElementsModule::findModuleFileByName(const std::string &name) const{
    auto it = m_d->fileModules.find(name);
    if ( it != m_d->fileModules.end() ){
        return it->second;
    }
    return nullptr;
}

ModuleFile *ElementsModule::moduleFileBypath(const std::string &path) const{
    for ( auto it = m_d->fileModules.begin(); it != m_d->fileModules.end(); ++it ){
        ModuleFile* mf = it->second;
        if ( mf->filePath() == path )
            return mf;
    }
    return nullptr;
}

const Module::Ptr& ElementsModule::module() const{
    return m_d->module;
}

void ElementsModule::compile(){
    if ( m_d->isCompiled )
        return;

    // resolve types
    if ( !m_d->typesResolved ){
        for ( auto it = m_d->fileModules.begin(); it != m_d->fileModules.end(); ++it ){
            it->second->resolveTypes();
        }
        m_d->typesResolved = true;
    }

    // compile dependencies
    for ( auto it = m_d->fileModules.begin(); it != m_d->fileModules.end(); ++it ){
        ModuleFile* mf = it->second;

        auto mfImports = mf->imports();
        for ( auto mit = mfImports.begin(); mit != mfImports.end(); ++mit ){
            if ( mit->module == nullptr ){
                THROW_EXCEPTION(lv::Exception, "Import not resolved: " + mit->uri, lv::Exception::toCode("~Import"));
            }
            mit->module->compile();
        }
    }

    for ( auto it = m_d->fileModules.begin(); it != m_d->fileModules.end(); ++it ){
        it->second->compile();
    }

    auto assets = m_d->module->assets();
    std::string moduleBuildPath = m_d->compiler->moduleBuildPath(m_d->module);

    for ( auto it = assets.begin(); it != assets.end(); ++it ){
        QString assetPath = QString::fromStdString(m_d->module->path() + "/" + *it);
        QString resultPath = QString::fromStdString(moduleBuildPath + "/" + *it);
        if (QFile::exists(resultPath))
            QFile::remove(resultPath);
        QFile::copy(assetPath, resultPath);
    }

    m_d->isCompiled = true;
}

Compiler::Ptr ElementsModule::compiler() const{
    return m_d->compiler;
}

Engine *ElementsModule::engine() const{
    return m_d->engine;
}

ElementsModule::Export ElementsModule::findExport(const std::string &name) const{
    auto exp = m_d->exports.find(name);
    if ( exp == m_d->exports.end() ){
        return ElementsModule::Export();
    }
    return exp->second;
}

const std::list<ModuleLibrary *> &ElementsModule::libraryModules() const{
    return m_d->libraries;
}

}} // namespace lv, el
