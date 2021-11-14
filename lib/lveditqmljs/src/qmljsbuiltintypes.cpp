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

#include "qmljsbuiltintypes_p.h"
#include "qmljs/qmljsvalueowner.h"

namespace lv{

QmlJsBuiltinTypes::QmlJsBuiltinTypes(){
}

QString QmlJsBuiltinTypes::nameFromCpp(const QString &type){
    QQmlJS::ValueOwner* valueowner = new QQmlJS::ValueOwner(QQmlJS::ValueOwner::sharedValueOwner());

    if (type == QLatin1String("QByteArray")
            || type == QLatin1String("QString")) {
        return "string";
    } else if (type == QLatin1String("QUrl")) {
        return "url";
    } else if (type == QLatin1String("long")) {
        return "long";
    }  else if (type == QLatin1String("float")
                || type == QLatin1String("qreal")) {
        return "double";
    } else if (type == QLatin1String("QFont")) {
        return valueowner->qmlFontObject()->className();
    } else if (type == QLatin1String("QPoint")
            || type == QLatin1String("QPointF")
            || type == QLatin1String("QVector2D")) {
        return valueowner->qmlPointObject()->className();
    } else if (type == QLatin1String("QSize")
            || type == QLatin1String("QSizeF")) {
        return valueowner->qmlSizeObject()->className();
    } else if (type == QLatin1String("QRect")
            || type == QLatin1String("QRectF")) {
        return valueowner->qmlRectObject()->className();
    } else if (type == QLatin1String("QVector3D")) {
        return valueowner->qmlVector3DObject()->className();
    } else if (type == QLatin1String("QColor")) {
        return "color";
    } else if (type == QLatin1String("QDateTime")) {
        return "date";
    }

    return "";
}

}// namespace
