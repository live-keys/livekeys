#include "library.h"
#include "live/utf8.h"
#include "live/path.h"
#include "live/exception.h"

#include <dlfcn.h>
#include <errno.h>
#include <string.h>
#include <locale.h>

namespace lv{

class LibraryPrivate{
public:
    std::string path;
    std::string name;
    void*       handle;
};


std::string Library::extensionImpl(){
#ifdef PLATFORM_OS_MAC
    return "dylib";
#else
    return "so";
#endif
}

Library::Ptr Library::loadImpl(const std::string &path){
    std::string pathWithSUffix = Path::suffix(path) == "" ? path + "." + extensionImpl() : path;

    dlerror(); /* Reset error status. */

    void* handle = dlopen(pathWithSUffix.c_str(), RTLD_LAZY);
    if ( !handle ){
        const char* msg = dlerror();
        if ( msg ){
            THROW_EXCEPTION(lv::Exception, Utf8("Failed to load library: %. %").format(pathWithSUffix, msg), lv::Exception::toCode("~Library"));
        } else {
            THROW_EXCEPTION(lv::Exception, Utf8("Failed to load library: %. Unknown error.").format(pathWithSUffix), lv::Exception::toCode("~Library"));
        }
    }

    LibraryPrivate* d = new LibraryPrivate;
    d->handle = handle;
    d->path = pathWithSUffix;
    d->name = Path::baseName(path);

    Library* lib = new Library;
    lib->m_d = d;
    return Library::Ptr(lib);
}

void Library::handleReferenceImpl(const std::string &){
}

bool Library::closeImpl(){
    if ( dlclose(m_d->handle) == 0){ // success
        m_d->handle = nullptr;
        return true;
    }
    return false;
}

void Library::cleanImpl(){
    closeImpl();
    delete m_d;
    m_d = nullptr;
}

void *Library::symbolImpl(const char *symbol){
    dlerror(); /* Reset error status. */

    void* ptr = dlsym(m_d->handle, symbol);
    if ( !ptr ){
        const char* msg = dlerror();
        THROW_EXCEPTION(lv::Exception, Utf8("Failed to load symbol: %. %").format(symbol, msg), lv::Exception::toCode("~Library"));
    }

    return ptr;
}

const std::string &Library::pathImpl() const{
    return m_d->path;
}

const std::string &Library::nameImpl() const{
    return m_d->name;
}




}// namespace
