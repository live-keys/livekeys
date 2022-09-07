#include "directory.h"
#include "live/visuallog.h"
#include "live/path.h"
#include "live/utf8.h"
#include "live/exception.h"

#include <filesystem>

namespace fs = std::filesystem;

namespace lv{

// Directory::IteratorPrivate
// ----------------------------------------------------------------------------

class Directory::IteratorPrivate{
public:
    fs::directory_iterator dit;
    fs::directory_iterator end;
    std::string path;
};


// Directory::Iterator
// ----------------------------------------------------------------------------

Directory::Iterator::Iterator(const std::string &path)
    : m_d(new Directory::IteratorPrivate)
{
    m_d->path = path;
    m_d->dit = fs::directory_iterator(path);
}

Directory::Iterator::Iterator(const Directory::Iterator &other)
    : m_d(new Directory::IteratorPrivate)
{
    m_d->path = other.m_d->path;
    m_d->dit = other.m_d->dit;
}

Directory::Iterator &Directory::Iterator::operator =(const Directory::Iterator &other){
    m_d->path = other.m_d->path;
    m_d->dit = other.m_d->dit;
    return *this;
}

Directory::Iterator::~Iterator(){
    delete m_d;
}

void Directory::Iterator::next(){
    ++m_d->dit;
}

bool Directory::Iterator::isEnd() const{
    return m_d->dit == m_d->end;
}

bool Directory::Iterator::hasEntry() const{
    return !isEnd();
}

std::string Directory::Iterator::path(){
    return m_d->dit->path();
}

// Directory
// ----------------------------------------------------------------------------

Directory::Directory(){}

Directory::Iterator Directory::iterate(const std::string &path){
    if ( !Path::exists(path) ){
        THROW_EXCEPTION(lv::Exception, Utf8("Failed to iterate directory: %. Path does not exist.").format(path), lv::Exception::toCode("~Path"));
    }
    if ( !Path::isDir(path) ){
        THROW_EXCEPTION(lv::Exception, Utf8("Failed to iterate directory: %. Path is not a directory.").format(path), lv::Exception::toCode("~Path"));
    }
    return Directory::Iterator(path);
}


}// namespace
