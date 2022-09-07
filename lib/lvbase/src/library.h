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
    class Table;

public:
    ~Library();

    static Ptr load(const std::string& path);
    static Ptr isLoaded(const std::string& path);
    void close();

    void* symbol(const std::string& name);
    void* symbol(const char* name);
    template<typename T> T symbolAs(const std::string& name);
    template<typename T> T symbolAs(const char* name);

    const std::string& path() const;
    const std::string& name() const;

    static void handleReference(const std::string& path);

private:
    static std::string extensionImpl();
    static Library::Ptr loadImpl(const std::string& path);
    static void handleReferenceImpl(const std::string& path);
    bool closeImpl();
    void cleanImpl();
    void* symbolImpl(const char* symbol);
    const std::string& pathImpl() const;
    const std::string& nameImpl() const;

private:
    static Library::Table& table();

private:
    Library();
    DISABLE_COPY(Library);

    LibraryPrivate* m_d;
};

inline void *Library::symbol(const std::string &name){
    return symbol(name.c_str());
}

inline void *Library::symbol(const char *name){
    return symbolImpl(name);
}

inline void Library::handleReference(const std::string &path){
    Library::handleReferenceImpl(path);
}

inline const std::string &Library::path() const{
    return pathImpl();
}

inline const std::string& Library::name() const{
    return nameImpl();
}

template<typename T> T Library::symbolAs(const std::string& name){
    return reinterpret_cast<T>(symbol(name));
}

template<typename T> T Library::symbolAs(const char* name){
    return reinterpret_cast<T>(symbol(name));
}

}// namespace

#endif // LVLIBRARY_H
