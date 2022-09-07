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

#include "library.h"
#include "live/exception.h"
#include "live/visuallog.h"

namespace lv{

class Library::Table{
public:
    std::map<std::string, Library::Ptr> loadedLibraries;
};


Library::Library()
    : m_d(nullptr)
{
}

Library::~Library(){
    cleanImpl();
}

void Library::close(){
    auto it = table().loadedLibraries.find(path());
    if ( it != table().loadedLibraries.end() )
        table().loadedLibraries.erase(it);
    closeImpl();
}

Library::Table &Library::table(){
    static Library::Table tb;
    return tb;
}


Library::Ptr lv::Library::load(const std::string &path){
    std::string fullPath = Path::resolve(path);
    Library::Ptr l = isLoaded(path);
    if ( l )
        return l;

    l = loadImpl(fullPath);
    table().loadedLibraries[fullPath] = l;
    return l;
}

Library::Ptr Library::isLoaded(const std::string &path){
    Library::Table& t = table();
    auto it = t.loadedLibraries.find(path);
    if ( it == t.loadedLibraries.end() )
        return nullptr;
    return it->second;
}

}// namespace
