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
