#include "modulelibrary.h"
#include "moduleloader.h"
#include "element_p.h"

#include <QLibrary>
#include <QString>

namespace lv{ namespace el{

class ModuleLibraryPrivate{
public:
    std::string   path;
    QLibrary*     library;
    ModuleLoader* loader;
    Engine*       engine;
};

ModuleLibrary::~ModuleLibrary(){
    if ( m_d->library ){
        ModuleLoader::DestroyLoaderFunction deleteFunction =
            reinterpret_cast<ModuleLoader::DestroyLoaderFunction>(m_d->library->resolve("destroyModuleLoader"));

        if ( !deleteFunction )
            vlog().e() << "Failed to find delete function for ModuleLoader in :" << m_d->path << ". This may result in memory leaks";

        deleteFunction(m_d->loader);
        delete m_d->library;
    }

    delete m_d;
}

ModuleLibrary* ModuleLibrary::create(Engine* engine, const std::string &path){
    return new ModuleLibrary(engine, path);
}

ModuleLibrary *ModuleLibrary::load(Engine* engine, const std::string &path){
    ModuleLibrary* ml = create(engine, path);

    QLibrary* library = new QLibrary(QString::fromStdString(path));
    ml->m_d->library = library;

    ModuleLoader::CreateLoaderFunction loaderFunction =
        reinterpret_cast<ModuleLoader::CreateLoaderFunction>(library->resolve("createModuleLoader"));

    if ( !loaderFunction )
        THROW_EXCEPTION(lv::Exception, "Failed to find loader function in library: " + path, 1);

    ml->m_d->loader = loaderFunction();
    if ( ml->m_d->loader == nullptr )
        THROW_EXCEPTION(lv::Exception, "Loader function returned ModuleLoader in :"  + path, 2);

    ml->m_d->loader->load(ml);

    return ml;
}

void ModuleLibrary::addInstance(const std::string &name, Element *element){
    m_instances[name] = element;
}

Engine *ModuleLibrary::engine(){
    return m_d->engine;
}

const std::string &ModuleLibrary::path() const{
    return m_d->path;
}

ModuleLibrary::ModuleLibrary(Engine* engine, const std::string &path)
    : m_d(new ModuleLibraryPrivate)
{
    m_d->path = path;
    m_d->library = nullptr;
    m_d->engine = engine;
}

}} // namespace lv, el
