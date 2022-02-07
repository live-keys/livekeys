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

#include <QLibrary>

namespace lv{

class LibraryPrivate{
public:
    LibraryPrivate(const QString& path) : lib(path){}

    QLibrary lib;
};

Library::Ptr lv::Library::load(const std::string &path){
    //TODO: Search library table for duplicates
    Library::Ptr l(new Library(path));
    if ( !l->m_d->lib.load() ){
        THROW_EXCEPTION(
            lv::Exception,
            "Failed to load library \'" + path + "\': " + l->m_d->lib.errorString().toStdString(),
            Exception::toCode("~Library")
        );
    }
    return l;
}

void *Library::symbol(const std::string &name){
    return symbol(name.c_str());
}

void *Library::symbol(const char *name){
    return reinterpret_cast<void*>(m_d->lib.resolve(name));
}

void Library::handleReference(const std::string &path){
#if defined(_MSC_VER) && !defined(__INTEL_COMPILER)
    QLibrary lib(QString::fromStdString(path));
    lib.load();
#endif
}

Library::Library(const std::string &path)
    : m_d(new LibraryPrivate(QString::fromStdString(path)))
{
}

}// namespace
