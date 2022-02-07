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

#include "qmlmetainfo_p.h"
#include "live/viewengine.h"
#include "live/qmllanguageinfo.h"

#include <QQmlEngine>
#include <QQmlProperty>

namespace lv {

QmlMetaInfo::QmlMetaInfo(QObject *parent)
    : QObject(parent)
{
}

QmlMetaInfo::~QmlMetaInfo(){
}

bool QmlMetaInfo::isCppOwned(QObject *obj){
    return QQmlEngine::objectOwnership(obj) == QQmlEngine::CppOwnership;
}

bool QmlMetaInfo::isJsOwned(QObject *obj){
    return QQmlEngine::objectOwnership(obj) == QQmlEngine::JavaScriptOwnership;
}

QObject* QmlMetaInfo::cppParent(QObject *obj){
    return obj->parent();
}

bool QmlMetaInfo::isWritable(QObject *obj, QString name){
    return QQmlProperty(obj, name).isWritable();
}

QJSValue QmlMetaInfo::listProperties(QObject *obj){
    QQmlEngine* engine = qobject_cast<QQmlEngine*>(parent());
    if ( !engine )
        return QJSValue();

    const QMetaObject* meta = obj->metaObject();

    QList<QString> properties;

    for ( int i = 0; i < meta->propertyCount(); i++ ){
        QMetaProperty property = meta->property(i);
        properties.push_back(property.name());
    }

    QJSValue v = engine->newArray(properties.size());
    for ( int i = 0; i < properties.length(); ++i ){
        v.setProperty(i, properties[i]);
    }
    return v;
}

QString QmlMetaInfo::defaultTypeValue(const QString &type){
    return QmlTypeInfo::typeDefaultValueAsText(type);
}

QString QmlMetaInfo::typeName(const QString &typeReference){
    return QmlTypeReference::split(typeReference).name();
}

}
