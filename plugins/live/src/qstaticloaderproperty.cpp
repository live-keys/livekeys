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

#include "qstaticloaderproperty.h"
#include <QQmlEngine>
#include <QtQml>


QStaticLoaderProperty::QStaticLoaderProperty(QObject *parent)
    : QObject(parent)
{
}

QStaticLoaderProperty::~QStaticLoaderProperty(){
    if ( m_value.canConvert<QObject*>() ){
        QObject* valueObj = m_value.value<QObject*>();
        valueObj->deleteLater();
    }
}

void QStaticLoaderProperty::setValue(const QVariant& value){
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

