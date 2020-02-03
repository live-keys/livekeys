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
