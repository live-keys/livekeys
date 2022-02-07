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

#ifndef LVPROGRAM_H
#define LVPROGRAM_H

#include "live/lvbaseglobal.h"

namespace lv{

class Utf8;
class LV_BASE_EXPORT Program{

public:
    virtual const Utf8& rootPath() const = 0;

    Program();
    virtual ~Program();
};

}// namespace

#endif // PROGRAM_H
