#include "library.h"
#include "live/exception.h"

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

Library::Library(const std::string &path)
    : m_d(new LibraryPrivate(QString::fromStdString(path)))
{
}

}// namespace
