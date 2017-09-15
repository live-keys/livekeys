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

#ifndef LVINCUBATIONCONTROLLER_H
#define LVINCUBATIONCONTROLLER_H

#include <QObject>
#include <QQmlIncubationController>

#include "live/lvbaseglobal.h"

namespace lcv{

class LVBASE_EXPORT IncubationController : public QObject, public QQmlIncubationController{

    Q_OBJECT

public:
    IncubationController(QObject* parent = 0);
    ~IncubationController();

protected:
    virtual void timerEvent(QTimerEvent*);
};

inline void IncubationController::timerEvent(QTimerEvent *){
    incubateFor(14);
}

}// namespace

#endif // LVINCUBATIONCONTROLLER_H
