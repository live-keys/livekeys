#include "elementsmodule.h"
#include "modulefile.h"
#include "modulelibrary.h"

#include "live/plugincontext.h"
#include "live/exception.h"
#include "live/elements/engine.h"

#include "v8nowarnings.h"

namespace lv{ namespace el{

/**
 * \class ElementsPlugin
 * \brief Container for a plugin functionality on the Elements side.
 *
 * Plugins are cached within the Engine class, so they require initialization through the Engine::require
 * function. The plugins are cached based on their importkey, for example ```import opencv.core``` will be cached
 * as <i>opencv.core</i>.
 */


class ElementsPluginPrivate{
public:
    ElementsPluginPrivate(Engine* e)
        : engine(e), libraryExports(Object::create(e)), typesResolved(false), compiled(false){}

    Plugin::Ptr plugin;
    Engine*     engine;
    std::map<std::string, ModuleFile*> fileModules;
    std::map<std::string, ElementsModule::Export> exports;

    Object      libraryExports;
    std::map<std::string, ModuleFile*> fileExports;

    std::list<ModuleLibrary*>          libraries;

    bool        typesResolved;
    bool        compiled;
};


ElementsModule::ElementsModule(Plugin::Ptr plugin, Engine *engine)
    : m_d(new ElementsPluginPrivate(engine))
{
    m_d->plugin = plugin;
}


ElementsModule::~ElementsModule(){
    for ( auto it = m_d->libraries.begin(); it != m_d->libraries.end(); ++it ){
        delete *it;
    }
    m_d->libraries.clear();
    delete m_d;
}

ElementsModule::Ptr ElementsModule::create(Plugin::Ptr plugin, Engine *engine){
    ElementsModule::Ptr epl(new ElementsModule(plugin, engine));

    for ( auto it = plugin->modules().begin(); it != plugin->modules().end(); ++it ){
        ElementsModule::addModuleFile(epl, *it + ".lv");
    }

    for ( auto it = plugin->libraryModules().begin(); it != plugin->libraryModules().end(); ++it ){
        ModuleLibrary* lib = ModuleLibrary::load(epl->m_d->engine, *it);
        lib->loadExports(epl->m_d->libraryExports);
        epl->m_d->libraries.push_back(lib);
    }

    return epl;
}

ModuleFile *ElementsModule::addModuleFile(ElementsModule::Ptr &epl, const std::string &name){
    auto it = epl->m_d->fileModules.find(name);
    if ( it != epl->m_d->fileModules.end() ){
        return it->second;
    }

    std::string filePath = epl->plugin()->path() + "/" + name;
    std::string content = epl->m_d->engine->fileInterceptor()->readFile(filePath);

    LanguageParser::AST* ast = epl->m_d->engine->parser()->parse(content);

    std::string componentName = name;
    size_t i = componentName.find(".lv");
    if ( i != std::string::npos ){
        componentName = name.substr(0, i);
    }

    ProgramNode* pn = epl->m_d->engine->compiler()->parseProgramNodes(componentName, ast);

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
            if ( epl->plugin()->context()->package == nullptr ){
                THROW_EXCEPTION(lv::Exception, "Cannot import relative path withouth package: " + imp.uri, Exception::toCode("~Import"));
            }

            std::string importUri = epl->plugin()->context()->package->name() + imp.uri;
            ElementsModule::Ptr ep = epl->m_d->engine->require(importUri, epl->plugin());
            if ( !ep ){
                THROW_EXCEPTION(lv::Exception, "Failed to find module: " + imp.uri, Exception::toCode("~Import"));
            }

            mf->resolveImport(imp.uri, ep);

        } else {
            ElementsModule::Ptr ep = epl->m_d->engine->require(it->uri, epl->plugin());
            if ( !ep ){
                THROW_EXCEPTION(lv::Exception, "Failed to find module: " + imp.uri, Exception::toCode("~Import"));
            }

            mf->resolveImport(imp.uri, ep);
        }
    }

    return mf;
}

void ElementsModule::addModuleLibrary(ModuleLibrary *library){
    m_d->libraries.push_back(library);
    library->loadExports(m_d->libraryExports);
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

const Plugin::Ptr& ElementsModule::plugin() const{
    return m_d->plugin;
}

Object ElementsModule::collectExportsObject(){
    auto isolate = m_d->engine->isolate();
    auto context = isolate->GetCurrentContext();
    v8::Local<v8::Object> result = v8::Object::New(isolate);

    v8::Local<v8::Object> lo = m_d->libraryExports.data();
    v8::Local<v8::Array> pn = lo->GetOwnPropertyNames(context, v8::ALL_PROPERTIES).ToLocalChecked();

    for (uint32_t i = 0; i < pn->Length(); ++i) {
        v8::Local<v8::Value> key = pn->Get(context, i).ToLocalChecked();
        v8::Local<v8::Value> value = lo->Get(context, key).ToLocalChecked();

        result->Set(context, key, value).IsNothing();
    }

    //TODO: Capture file exports

    return Object(m_d->engine, result);
}

const Object &ElementsModule::libraryExports() const{
    return m_d->libraryExports;
}

void ElementsModule::compile(){
    if ( m_d->compiled )
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

    m_d->compiled = true;
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

const std::map<std::string, ModuleFile *> &ElementsModule::fileExports() const{
    return m_d->fileExports;
}

const std::list<ModuleLibrary *> &ElementsModule::libraryModules() const{
    return m_d->libraries;
}

}} // namespace lv, el
