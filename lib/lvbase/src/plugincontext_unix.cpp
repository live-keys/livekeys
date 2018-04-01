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

#include <errno.h>
#include <unistd.h>
#include <stdio.h>
#include <utime.h>
#include <cstring>
#include <linux/limits.h>

namespace lv{

QString PluginContext::applicationFilePathImpl(){
    char link[PATH_MAX];
    ssize_t linkSize;
    if ( ( linkSize = readlink("/proc/self/exe", link, sizeof(link) - 1) ) != -1 ){
        link[linkSize] = '\0';
        return QString(link);
    } else {
        return QString();
    }
}

} // namespace
