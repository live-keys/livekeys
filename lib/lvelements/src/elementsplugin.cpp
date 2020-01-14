#include "elementsplugin.h"
#include "modulefile.h"
#include "modulelibrary.h"

#include "live/plugincontext.h"
#include "live/exception.h"

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
    Plugin::Ptr plugin;
    Engine*     engine;
    std::map<std::string, ModuleFile*> fileModules;
    std::map<std::string, ModuleLibrary*> libraryModules;

    std::map<std::string, ModuleFile*> fileExports;
};


ElementsPlugin::~ElementsPlugin(){
    for ( auto it = m_d->libraryModules.begin(); it != m_d->libraryModules.end(); ++it ){
        delete it->second;
    }
    m_d->libraryModules.clear();
    delete m_d;
}

ElementsPlugin::Ptr ElementsPlugin::create(Plugin::Ptr plugin, Engine *engine){
    ElementsPlugin::Ptr epl(new ElementsPlugin(plugin, engine));

    for ( auto it = plugin->modules().begin(); it != plugin->modules().end(); ++it ){
        epl->m_d->fileModules[*it] = epl->load(epl, *it);
    }
    for ( auto it = plugin->libraryModules().begin(); it != plugin->libraryModules().end(); ++it ){
        epl->m_d->libraryModules[*it] = epl->loadLibrary(epl, *it);
    }

    return epl;
}

ModuleFile *ElementsPlugin::addModuleFile(ElementsPlugin::Ptr &epl, const std::string &name){
    ModuleFile* mf = new ModuleFile(epl, name);
    mf->initializeImportsExports(epl->m_d->engine);
    epl->m_d->fileModules[name] = mf;
    return mf;
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

ModuleLibrary *ElementsPlugin::loadLibrary(const ElementsPlugin::Ptr &, const std::string &name){
    ModuleLibrary* lib = ModuleLibrary::load(m_d->engine, name);
    lib->initializeExports();
    return lib;
}

const Plugin::Ptr& ElementsPlugin::plugin() const{
    return m_d->plugin;
}

const std::map<std::string, ModuleFile *> &ElementsPlugin::fileExports() const{
    return m_d->fileExports;
}

ElementsPlugin::ElementsPlugin(Plugin::Ptr plugin, Engine *engine) : m_d(new ElementsPluginPrivate){
    m_d->plugin = plugin;
    m_d->engine = engine;
}

}} // namespace lv, el
