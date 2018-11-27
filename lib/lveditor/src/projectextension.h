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

#ifndef LVCODEHANDLERCATEGORY_H
#define LVCODEHANDLERCATEGORY_H

#include "live/lveditorglobal.h"
#include "live/abstractcodehandler.h"

namespace lv{

class Project;
class Settings;
class ViewEngine;
class LV_EDITOR_EXPORT ProjectExtension : public QObject{

    Q_OBJECT

public:
    ProjectExtension(QObject* parent = 0);
    virtual ~ProjectExtension();

    virtual AbstractCodeHandler* createHandler(
        ProjectDocument* document,
        Project* project,
        ViewEngine* engine,
        DocumentHandler* parent
    ) = 0;
};

}// namespace

#endif // LVCODEHANDLERCATEGORY_H
