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

inline LanguageUtils::FakeMetaObject::Ptr metaObjectFromQmlObject(const DocumentQmlObject& object){
    LanguageUtils::FakeMetaObject::Ptr fmo(new LanguageUtils::FakeMetaObject);
    fmo->setClassName(object.typeName());
    fmo->setAttachedTypeName(object.typeName());

    for (
        QMap<QString, QString>::const_iterator it = object.memberProperties().begin();
        it != object.memberProperties().end();
        ++it )
    {
        LanguageUtils::FakeMetaProperty fmp(
            it.key(), it.value(), false, true, DocumentQmlInfo::isObject(it.value()), 0
        );
        fmo->addProperty(fmp);
    }

    for (
        QMap<QString, DocumentQmlObject::FunctionValue>::const_iterator it = object.memberFunctions().begin();
        it != object.memberFunctions().end();
        ++it
    )
    {
        LanguageUtils::FakeMetaMethod fmm(it.value().name);
        fmm.setMethodType(LanguageUtils::FakeMetaMethod::Method);

        for (
            QList<QPair<QString, QString> >::const_iterator argit = it.value().arguments.begin();
            argit != it.value().arguments.end();
            ++argit
        ){
            fmm.addParameter(argit->first, argit->second);
        }

        fmo->addMethod(fmm);
    }

    for (
        QMap<QString, DocumentQmlObject::FunctionValue>::const_iterator it = object.memberSignals().begin();
        it != object.memberSignals().end();
        ++it
    )
    {
        LanguageUtils::FakeMetaMethod fmm(it.value().name);
        fmm.setMethodType(LanguageUtils::FakeMetaMethod::Signal);

        for (
            QList<QPair<QString, QString> >::const_iterator argit = it.value().arguments.begin();
            argit != it.value().arguments.end();
            ++argit
        ){
            fmm.addParameter(argit->first, argit->second);
        }

        fmo->addMethod(fmm);
    }

    for (
        QMap<QString, QString>::const_iterator it = object.memberSlots().begin();
        it != object.memberSlots().end();
        ++it )
    {
        LanguageUtils::FakeMetaMethod fmm(it.key(), it.value());
        fmm.setMethodType(LanguageUtils::FakeMetaMethod::Slot);
        fmo->addMethod(fmm);
    }

    return fmo;
}

} // namespace

#endif // LVDOCUMENTQMLOBJECT_P_H
