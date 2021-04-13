#include "elementsplugin.h"
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
    ElementsPluginPrivate(Engine* e) : engine(e), libraryExports(Object::create(e)){}

    Plugin::Ptr plugin;
    Engine*     engine;
    Object      libraryExports;
    std::map<std::string, ModuleFile*> fileModules;
    std::map<std::string, ModuleFile*> fileExports;

    std::list<ModuleLibrary*>          libraries;
};


ElementsPlugin::ElementsPlugin(Plugin::Ptr plugin, Engine *engine)
    : m_d(new ElementsPluginPrivate(engine))
{
    m_d->plugin = plugin;
}


ElementsPlugin::~ElementsPlugin(){
    for ( auto it = m_d->libraries.begin(); it != m_d->libraries.end(); ++it ){
        delete *it;
    }
    m_d->libraries.clear();
    delete m_d;
}

ElementsPlugin::Ptr ElementsPlugin::create(Plugin::Ptr plugin, Engine *engine){
    ElementsPlugin::Ptr epl(new ElementsPlugin(plugin, engine));

    for ( auto it = plugin->modules().begin(); it != plugin->modules().end(); ++it ){
        epl->m_d->fileModules[*it] = epl->load(epl, *it);
    }

    for ( auto it = plugin->libraryModules().begin(); it != plugin->libraryModules().end(); ++it ){
        ModuleLibrary* lib = ModuleLibrary::load(epl->m_d->engine, *it);
        lib->loadExports(epl->m_d->libraryExports);
        epl->m_d->libraries.push_back(lib);
    }

    return epl;
}

ModuleFile *ElementsPlugin::addModuleFile(ElementsPlugin::Ptr &epl, const std::string &name){
    ModuleFile* mf = new ModuleFile(epl, name);
    mf->initializeImportsExports(epl->m_d->engine);
    epl->m_d->fileModules[name] = mf;
    return mf;
}

void ElementsPlugin::addModuleLibrary(ModuleLibrary *library){
    m_d->libraries.push_back(library);
    library->loadExports(m_d->libraryExports);
}

ModuleFile *ElementsPlugin::load(const ElementsPlugin::Ptr& epl, const std::string &name){
    ModuleFile* mf = new ModuleFile(epl, name);
    mf->parse(m_d->engine);
    m_d->fileModules[name] = mf;

    for ( auto it = mf->exports().begin(); it != mf->exports().end(); ++it ){
        auto storedExport = m_d->fileExports.find(*it);
        if ( storedExport != m_d->fileExports.end() ){
            THROW_EXCEPTION(
                lv::Exception,
                "Multiple exports defined for the same module: " + *it + " in " + m_d->plugin->context()->importId,
                1);
        }
        m_d->fileExports[*it] = mf;
    }

    vlog("lvelements-plugin").v() << "Plugin \'" << m_d->plugin->path() << "\' loaded module " << name;

    return mf;
}

const Plugin::Ptr& ElementsPlugin::plugin() const{
    return m_d->plugin;
}

Object ElementsPlugin::collectExportsObject(){
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

const Object &ElementsPlugin::libraryExports() const{
    return m_d->libraryExports;
}

const std::map<std::string, ModuleFile *> &ElementsPlugin::fileExports() const{
    return m_d->fileExports;
}

const std::list<ModuleLibrary *> &ElementsPlugin::libraryModules() const{
    return m_d->libraries;
}

}} // namespace lv, el
