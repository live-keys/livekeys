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


#include "live/qmlobjectcodeserializer.h"
#include "live/documentqmlfragment.h"

namespace lv{

QmlObjectCodeSerializer::QmlObjectCodeSerializer(QObject *parent)
    : AbstractCodeSerializer(parent)
{
}

QmlObjectCodeSerializer::~QmlObjectCodeSerializer(){

}

QString QmlObjectCodeSerializer::toCode(const QVariant &value, const DocumentEditFragment* channel){
    const DocumentQmlFragment* qmlfragment = static_cast<const DocumentQmlFragment*>(channel);
    if ( qmlfragment ){
        if ( qmlfragment->property().propertyTypeCategory() == QQmlProperty::Object ){
            qmlfragment->property().write(value);
        }
    }

    return QString();
}

QVariant QmlObjectCodeSerializer::fromCode(const QString &code, const DocumentEditFragment *channel){
    const DocumentQmlFragment* qmlfragment = static_cast<const DocumentQmlFragment*>(channel);
    if ( qmlfragment ){
        if ( qmlfragment->property().propertyTypeCategory() == QQmlProperty::Object ){
            return qmlfragment->property().read();
        }
    }

    qWarning("Code section \'%s\' requires qml binding channel in order to modify value", qPrintable(code));
    return QVariant();
}

} // namespace
