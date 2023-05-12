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

#ifndef LVMETAOBJECTTYPEINFO_H
#define LVMETAOBJECTTYPEINFO_H

#include "live/elements/lvelengineglobal.h"
#include "live/elements/compiler/languageinfo.h"

#include <string>

namespace lv{ namespace el{

class MetaObject;

/**
 * @brief The MetaObject class
 */
class LV_ELEMENTS_ENGINE_EXPORT MetaObjectTypeInfo{

public:
    static TypeInfo::Ptr extract(const MetaObject& mo, const Utf8 &uri = "", bool isInstance = false, bool isCreatable = true);

};

}} // namespace lv, el

#endif // LVMETAOBJECT_H
