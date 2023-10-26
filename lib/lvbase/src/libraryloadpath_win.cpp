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


#include "live/libraryloadpath.h"
#include "live/visuallog.h"
#include "live/path.h"
#include "live/directory.h"

#include "Windows.h"

namespace lv{

// SetDefaultDllDirectories(
//   LOAD_LIBRARY_SEARCH_APPLICATION_DIR |
//   LOAD_LIBRARY_SEARCH_SYSTEM32 |
//   LOAD_LIBRARY_SEARCH_USER_DIRS
// )
// and AddDllDirectory() have weird behaviors when loading QtQuick.Controls, it's why we prefer to
// manipulate the 'PATH' environment variable instead.

namespace{

std::string pathContents;

}// namespace

void LibraryLoadPath::addImpl(const std::string& path, const std::string& , bool recursive){
    bool firstRun = false;

    if ( pathContents.empty() ){
        firstRun = true;

        const size_t bufferSize = 4096;
        char* buffer = reinterpret_cast<char*>(malloc(bufferSize));
        GetEnvironmentVariableA("PATH", buffer, bufferSize);
        pathContents = std::string(buffer);
        free(buffer);
    }


    Directory::Iterator dit = Directory::iterate(path);
    while ( !dit.isEnd() ){
        std::string current = dit.path();
        if ( Path::isDir(current)){
            std::string currentToWrite = current;
            Utf8::replaceAll(currentToWrite, "/", "\\");
            pathContents += ";" + currentToWrite;
            if ( recursive )
                addImpl(current, "", recursive);
        }
        dit.next();
    }

    if ( firstRun ){
        SetEnvironmentVariableA("PATH", pathContents.c_str());
        firstRun = false;
        pathContents.clear();
    }
}

}// namespace
