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

#ifndef QABSTRACTCODESERIALIZER_H
#define QABSTRACTCODESERIALIZER_H

#include <QObject>
#include <QJSValue>
#include "qlcveditorglobal.h"

namespace lcv{

class Q_LCVEDITOR_EXPORT QAbstractCodeSerializer : public QObject{

    Q_OBJECT

public:
    explicit QAbstractCodeSerializer(QObject *parent = 0);
    virtual ~QAbstractCodeSerializer();

    virtual QString toCode(const QVariant& value) = 0;
    virtual QVariant fromCode(const QString& value) = 0;

};

}// namespace

#endif // QABSTRACTCODESERIALIZER_H
