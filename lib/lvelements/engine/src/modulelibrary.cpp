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

#include "modulelibrary.h"
#include "moduleloader.h"
#include "element_p.h"
#include "live/library.h"

namespace lv{ namespace el{

class ModuleLibraryPrivate{
public:
    std::string   path;
    Library::Ptr  library;
    ModuleLoader* loader;
    Engine*       engine;
};

ModuleLibrary::~ModuleLibrary(){
    if ( m_d->library ){
        ModuleLoader::DestroyLoaderFunction deleteFunction =
            m_d->library->symbolAs<ModuleLoader::DestroyLoaderFunction>("destroyModuleLoader");

        if ( !deleteFunction ){
            vlog().e() << "ModuleLibrary: Failed to find delete function for ModuleLoader in :" << m_d->path << ". This may result in memory leaks";
            return;
        }

        deleteFunction(m_d->loader);
        m_d->library = nullptr;
    }

    delete m_d;
}

ModuleLibrary* ModuleLibrary::create(Engine* engine, const std::string &path){
    return new ModuleLibrary(engine, path);
}

ModuleLibrary *ModuleLibrary::load(Engine* engine, const std::string &path){
    ModuleLibrary* ml = create(engine, path);

    ml->m_d->library = Library::load(path);

    ModuleLoader::CreateLoaderFunction loaderFunction =
        ml->m_d->library->symbolAs<ModuleLoader::CreateLoaderFunction>("createModuleLoader");

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
