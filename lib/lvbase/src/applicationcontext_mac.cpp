/****************************************************************************
**
** Copyright (C) 2014-2019 Dinu SV.
** (contact: mail@dinusv.com)
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

#include "applicationcontext.h"
#include <QString>

#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <limits.h>
#include "mach-o/dyld.h"

namespace lv{

std::string ApplicationContext::applicationFilePathImpl(){
    uint32_t pathNameSize = 0;
    _NSGetExecutablePath(NULL, &pathNameSize);
    char relativePath[pathNameSize];
    if ( _NSGetExecutablePath(relativePath, &pathNameSize) == 0 ){
        char fullPath[PATH_MAX];
        if ( realpath(relativePath, fullPath) != NULL )
            return std::string(fullPath);
        else
            return std::string(relativePath);
    } else
        return std::string();
}

}// namespace

