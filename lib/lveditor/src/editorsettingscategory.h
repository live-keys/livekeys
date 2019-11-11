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

#ifndef LVEDITORSETTINGSCATEGORY_H
#define LVEDITORSETTINGSCATEGORY_H

#include <QJsonObject>

#include "live/lveditorglobal.h"

namespace lv{

class LV_EDITOR_EXPORT EditorSettingsCategory{

public:
    EditorSettingsCategory();
    virtual ~EditorSettingsCategory();

    virtual void fromJson(const QJsonValue&){}
    virtual QJsonValue toJson() const{ return QJsonValue(); }
};

}// namespace

#endif // LVEDITORSETTINGSCATEGORY_H
