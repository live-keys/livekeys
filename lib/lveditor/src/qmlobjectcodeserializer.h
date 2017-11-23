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

#ifndef LVQMLOBJECTCODESERIALIZER_H
#define LVQMLOBJECTCODESERIALIZER_H

#include <QObject>

#include "live/lveditorglobal.h"
#include "live/abstractcodeserializer.h"

namespace lv{

class LV_EDITOR_EXPORT QmlObjectCodeSerializer : public AbstractCodeSerializer{

public:
    explicit QmlObjectCodeSerializer(QObject* parent = 0);
    ~QmlObjectCodeSerializer();

    QString toCode(const QVariant &value, const DocumentEditFragment* channel) Q_DECL_OVERRIDE;
    QVariant fromCode(const QString& value, const DocumentEditFragment* channel) Q_DECL_OVERRIDE;

};

}// namespace

#endif // LVQMLOBJECTCODESERIALIZER_H
