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

#include "staticloaderproperty.h"
#include <QQmlEngine>
#include <QtQml>

namespace lv{

StaticLoaderProperty::StaticLoaderProperty(QObject *parent)
    : QObject(parent)
{
}

StaticLoaderProperty::~StaticLoaderProperty(){
    if ( m_value.canConvert<QObject*>() ){
        QObject* valueObj = m_value.value<QObject*>();
        valueObj->deleteLater();
    }
}

void StaticLoaderProperty::setValue(const QVariant& value){
    if (m_value == value)
        return;

    if ( m_value.canConvert<QObject*>() ){
        QObject* valueObj = m_value.value<QObject*>();
        qmlEngine(this)->setObjectOwnership(valueObj, QQmlEngine::JavaScriptOwnership);
    }

    if ( value.canConvert<QObject*>() ){
        QObject* valueObj = value.value<QObject*>();
        qmlEngine(this)->setObjectOwnership(valueObj, QQmlEngine::CppOwnership);
    }
    m_value = value;

    emit valueChanged();
}

}// namespace
