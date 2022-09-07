#include "library.h"
#include "live/utf8.h"
#include "live/path.h"
#include "live/exception.h"

#include <inttypes.h>
#include <Windows.h>


namespace lv{

class LibraryPrivate{
public:
    std::string path;
    std::string name;
    HMODULE     handle;

public:
    static std::string formatError(const std::string& filename, DWORD errorno);
};


std::string Library::extensionImpl(){
    return "dll";
}

Library::Ptr Library::loadImpl(const std::string &path){
    std::string pathWithSUffix = Path::suffix(path) == "" ? path + "." + extensionImpl() : path;

    WCHAR pathWithSuffixW[32768];


    if (!MultiByteToWideChar(CP_UTF8,
                             0,
                             pathWithSUffix,
                             -1,
                             pathWithSuffixW,
                             ARRAY_SIZE(pathWithSuffixW)))
    {
        std::string error = LibraryPrivate::formatError(path, GetLastError());
        THROW_EXCEPTION(lv::Exception, Utf8("Library load error: %").format(error), lv::Exception::toCode("~Library"));
    }

    HMODULE handle = LoadLibraryExW(pathWithSuffixW, NULL, LOAD_WITH_ALTERED_SEARCH_PATH);
    if (handle == NULL) {
        std::string error = LibraryPrivate::formatError(path, GetLastError());
        THROW_EXCEPTION(lv::Exception, Utf8("Library load error: %").format(error), lv::Exception::toCode("~Library"));
    }

    LibraryPrivate* d = new LibraryPrivate;
    d->handle = handle;
    d->path = path;
    d->name = Path::baseName(path);

    Library* lib = new Library;
    lib->m_d = d;
    return Library::Ptr(lib);
}

void Library::handleReferenceImpl(const std::string &path){
    Library::load(path);
}

bool Library::closeImpl(){
    if (FreeLibrary(m_d->handle)){ // success
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
    void *ptr = (void*)(uintptr_t)GetProcAddress(m_d->handle, symbol);
    if ( !ptr ){
        std::string error = LibraryPrivate::formatError(path, GetLastError());
        THROW_EXCEPTION(lv::Exception, Utf8("Library symbol error: %").format(error), lv::Exception::toCode("~Library"));
    }
    return ptr;
}

const std::string &Library::pathImpl() const{
    return m_d->path;
}

const std::string &Library::nameImpl() const{
    return m_d->name;
}

std::string LibraryPrivate::formatError(const std::string &filename, DWORD errorno){
    DWORD res;
    char* buffer;

    if (errorno == 0)
        return std::string();

    res = FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER |
                       FORMAT_MESSAGE_FROM_SYSTEM |
                       FORMAT_MESSAGE_IGNORE_INSERTS,
                       NULL,
                       errorno,
                       MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US),
                       (LPSTR)buffer, 0, NULL);

    if (!res && (GetLastError() == ERROR_MUI_FILE_NOT_FOUND ||
               GetLastError() == ERROR_RESOURCE_TYPE_NOT_FOUND))
    {
        res = FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER |
                             FORMAT_MESSAGE_FROM_SYSTEM |
                             FORMAT_MESSAGE_IGNORE_INSERTS,
                             NULL,
                             errorno,
                             0, (LPSTR) &buffer, 0, NULL);
    }

    if (res && errorno == ERROR_BAD_EXE_FORMAT ){
        char* msg = buffer;
        DWORD_PTR fileNameArg = (DWORD_PTR)filename.c_str();
        res = FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER |
                             FORMAT_MESSAGE_ARGUMENT_ARRAY |
                             FORMAT_MESSAGE_FROM_STRING,
                             msg,
                             0, 0, (LPSTR) &buffer, 0, (va_list*) &fileNameArg);
        LocalFree(msg);
    }

    if (!res)
        return std::string("Unknown error.");


    std::string result = std::string(buffer);
    LocalFree(buffer);

    return result;
}




}// namespace
