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

#ifndef LVDOCUMENTQMLOBJECT_P_H
#define LVDOCUMENTQMLOBJECT_P_H

#include "live/documentqmlobject.h"
#include "live/documentqmlinfo.h"
#include "languageutils/fakemetaobject.h"

namespace lv{

class QmlTypeInfoPrivate{

public:
    static QmlTypeInfo::Ptr fromMetaObject(LanguageUtils::FakeMetaObject::ConstPtr fmo, const QString& libraryUri = "");
    static QmlFunctionInfo fromMetaMethod(const QmlTypeInfo& parent, const LanguageUtils::FakeMetaMethod& method);
    static QmlPropertyInfo fromMetaProperty(const QmlTypeInfo& parent, const LanguageUtils::FakeMetaProperty& prop);
    static QmlEnumInfo fromMetaEnum(const LanguageUtils::FakeMetaEnum& prop);
};

} // namespace

#endif // LVDOCUMENTQMLOBJECT_P_H
