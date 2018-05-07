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

#ifndef LVFILTERWORKER_P_H
#define LVFILTERWORKER_P_H

#include <QObject>
#include <QCoreApplication>
#include "live/filterworker.h"

namespace lv{

class FilterWorkerPrivate : public QObject{

    Q_OBJECT

public:
    explicit FilterWorkerPrivate() : QObject(0){}
    ~FilterWorkerPrivate(){}

    void postNotify(const std::function<void ()>& fnc){
        QCoreApplication::postEvent(this, new FilterWorker::CallEvent(fnc));
    }

    void postNotify(FilterWorker::CallEvent* evt){
        QCoreApplication::postEvent(this, evt);
    }

    bool event(QEvent *event){
        if (!dynamic_cast<FilterWorker::CallEvent*>(event))
            return QObject::event(event);

        FilterWorker::CallEvent* ce = static_cast<FilterWorker::CallEvent*>(event);
        ce->callFilter();
        delete ce->locker();
        return true;
    }

private:


};

}// namespace

#endif // LVFILTERWORKER_P_H
