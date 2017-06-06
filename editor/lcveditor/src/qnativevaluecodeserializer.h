/****************************************************************************
**
** Copyright (C) 2014-2017 Dinu SV.
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

#ifndef QNATIVEVALUECODESERIALIZER_H
#define QNATIVEVALUECODESERIALIZER_H

#include <QObject>
#include <QJSValue>
#include "qlcveditorglobal.h"
#include "qabstractcodeserializer.h"

namespace lcv{

class Q_LCVEDITOR_EXPORT QNativeValueCodeSerializer : public QAbstractCodeSerializer{

    Q_OBJECT

public:
    explicit QNativeValueCodeSerializer(QObject *parent = 0);
    ~QNativeValueCodeSerializer();

    QString toCode(const QVariant& value);
    QVariant fromCode(const QString& value);
};

}// namespace

#endif // QNATIVEVALUECODESERIALIZER_H
