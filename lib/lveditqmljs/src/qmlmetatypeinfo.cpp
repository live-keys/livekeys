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

#include "qmlmetatypeinfo_p.h"

namespace lv{


QmlMetaTypeInfo::QmlMetaTypeInfo(const lv::QmlInheritanceInfo &typeInfo, ViewEngine* engine, QObject *parent)
    : QObject(parent)
    , m_inheritanceInfo(typeInfo)
    , m_engine(engine)
{
}


QmlMetaTypeInfo::~QmlMetaTypeInfo(){
}

QJSValue QmlMetaTypeInfo::typeName(const QString &typeReference){
    return QJSValue(QmlTypeReference::split(typeReference).name());
}

QJSValue QmlMetaTypeInfo::propertyInfo(const QString &propertyName){
    for ( auto it = m_inheritanceInfo.nodes.begin(); it != m_inheritanceInfo.nodes.end(); ++it ){
        QmlTypeInfo::Ptr ti = *it;
        QmlPropertyInfo pi = ti->propertyAt(propertyName);
        if ( pi.isValid() ){
            QJSValue result = m_engine->engine()->newObject();
            result.setProperty("isWritable", pi.isWritable);
            result.setProperty("isList", pi.isList);
            result.setProperty("name", pi.name);
            result.setProperty("type", pi.typeName.join());
            result.setProperty("parentType", pi.objectType.join());
            return result;
        }
    }
    return QJSValue();
}

} // namespace
