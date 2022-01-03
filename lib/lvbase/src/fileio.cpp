#include "fileio.h"
#include "live/exception.h"
#include <fstream>
#include <istream>

#include <QFile>
#include <QTextStream>

namespace lv{

FileIOInterface::FileIOInterface(){
}

FileIOInterface::~FileIOInterface(){
}

FileIO::FileIO(){
}

FileIO::~FileIO(){
}

std::string FileIO::readFromFile(const std::string &path){
    std::ifstream instream(path, std::ifstream::in | std::ifstream::binary);
    if ( !instream.is_open() ){
        THROW_EXCEPTION(lv::Exception, Utf8("Failed to open file: %").format(path), lv::Exception::toCode("~File"));
    }

    instream.seekg(0, std::ios::end);
    size_t size = static_cast<size_t>(instream.tellg());
    std::string buffer(size, ' ');
    instream.seekg(0);
    instream.read(&buffer[0], size);

    return buffer;
}

bool FileIO::writeToFile(const std::string &path, const std::string &content){
    return writeToFile(path, content.c_str(), content.length());
}

bool FileIO::writeToFile(const std::string &path, const char *content, size_t length){
    QFile fileInput(path.c_str());
    if ( !fileInput.open(QIODevice::WriteOnly ) ){
        THROW_EXCEPTION(lv::Exception, Utf8("Failed to open file for writing: %").format(path), lv::Exception::toCode("~File"));
    }

    fileInput.write(content, static_cast<int>(length));
    fileInput.close();

    return true;
}

}// namespace
