/****************************************************************************
**
** Copyright (C) 2014-2018 Dinu SV.
** (contact: mail@dinusv.com)
** This file is part of Live CV Application.
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

#include "plugincontext.h"
#include <QString>

#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <limits.h>
#include "mach-o/dyld.h"

namespace lv{

QString PluginContext::applicationFilePathImpl(){
    uint32_t pathNameSize = 0;
    _NSGetExecutablePath(NULL, &pathNameSize);
    char relativePath[pathNameSize];
    if ( _NSGetExecutablePath(relativePath, &pathNameSize) == 0 ){
        char fullPath[PATH_MAX];
        if ( realpath(relativePath, fullPath) != NULL )
            return QString(fullPath);
        else
            return QString(relativePath);
    } else
        return QString();
}

}// namespace

