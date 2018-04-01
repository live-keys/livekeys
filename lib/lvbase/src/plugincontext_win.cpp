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

#include <windows.h>
#include <tchar.h>

namespace lv{

QString PluginContext::applicationFilePathImpl(){
    char* buffer    = 0;
    char* newBuffer = 0;
    int   size      = 0;
    DWORD n         = 0;
    do{
        size += 256; // grow until fits
        newBuffer = (char*)realloc(buffer, size * sizeof(char));
        if ( newBuffer != NULL ){
            buffer = newBuffer;
        } else {
            free(buffer);
            return QString();
        }
        n = GetModuleFileNameA(NULL, buffer, size);
    } while ((int)n >= size);
    if ( buffer != 0 ){
        QString base(buffer);
        free(buffer);
        return base;
    }
    return QString();
}

} // namespace
