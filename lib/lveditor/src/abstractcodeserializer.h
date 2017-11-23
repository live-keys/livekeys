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

#ifndef LVABSTRACTCODESERIALIZER_H
#define LVABSTRACTCODESERIALIZER_H

#include <QObject>
#include <QJSValue>

#include "live/lveditorglobal.h"

namespace lv{

class DocumentEditFragment;
class LV_EDITOR_EXPORT AbstractCodeSerializer : public QObject{

    Q_OBJECT

public:
    explicit AbstractCodeSerializer(QObject *parent = 0);
    virtual ~AbstractCodeSerializer();

    virtual QString toCode(const QVariant& value, const DocumentEditFragment* channel) = 0;
    virtual QVariant fromCode(const QString& value, const DocumentEditFragment* channel) = 0;

};

}// namespace

#endif // LVABSTRACTCODESERIALIZER_H
