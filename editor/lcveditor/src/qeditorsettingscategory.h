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

#ifndef QEDITORSETTINGSCATEGORY_H
#define QEDITORSETTINGSCATEGORY_H

#include "qlcveditorglobal.h"
#include <QJsonObject>

namespace lcv{

class Q_LCVEDITOR_EXPORT QEditorSettingsCategory{

public:
    QEditorSettingsCategory();
    ~QEditorSettingsCategory();

    virtual void fromJson(const QJsonValue&){}
    virtual QJsonValue toJson() const{ return QJsonValue(); }
};

}// namespace

#endif // QEDITORSETTINGSCATEGORY_H
