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

namespace lv{

namespace{

bool assertLinkPathExists(const std::string& linkPath){
    if ( !Path::exists(linkPath) )
        return Path::createDirectory(linkPath);
    return true;
}

}// namespace

void LibraryLoadPath::addImpl(const std::string &path, const std::string &linkPath, bool recursive){
    if ( !assertLinkPathExists(linkPath) ){
        vlog("global").e() << "Failed to create link directory. Some library dependencies may fail to load.";
        return;
    }

    Directory::Iterator dit = Directory::iterate(path);
    while ( !dit.isEnd() ){
        std::string current = dit.path();

        if(  ( Path::isFile(current) || Path::isSymlink(current) ) &&
              ( current.find(".so") != std::string::npos || current.find(".dylib") != std::string::npos )
          )
        {
            std::string fileName = Path::name(current);
            if( fileName.find("lib") == 0 ){
                std::string fullLinkPath = Path::join(linkPath, fileName);
                if ( !Path::exists(fullLinkPath) )
                    Path::createSymlink(fullLinkPath, current);
                vlog_debug("libraryloadpath", "Added \'" + linkPath + "\' -> \'" + current + "\'");
            }
        } else if ( Path::isDir(current) && recursive ){
            addImpl(current, linkPath, recursive);
        }
        dit.next();
    }
}

}// namespace
