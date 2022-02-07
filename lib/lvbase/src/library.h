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

#ifndef LVLIBRARY_H
#define LVLIBRARY_H

#include "live/lvbaseglobal.h"
#include "live/utf8.h"

#include <memory>

namespace lv{

class LibraryPrivate;

class LV_BASE_EXPORT Library{

public:
    typedef std::shared_ptr<Library>       Ptr;
    typedef std::shared_ptr<const Library> ConstPtr;

public:
    static Ptr load(const std::string& path);
    void* symbol(const std::string& name);
    void* symbol(const char* name);

    template<typename T> T fn(const std::string& name);
    template<typename T> T fn(const char* name);

    static void handleReference(const std::string& path);

private:
    Library(const std::string& path);
    DISABLE_COPY(Library);

    LibraryPrivate* m_d;

};

template<typename T> T Library::fn(const std::string& name){
    return reinterpret_cast<T>(symbol(name));
}

template<typename T> T Library::fn(const char* name){
    return reinterpret_cast<T>(symbol(name));
}

}// namespace

#endif // LVLIBRARY_H
