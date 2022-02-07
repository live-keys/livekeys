/****************************************************************************
**
** Copyright (C) 2022 Dinu SV.
**
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

#ifndef LVFILTERWORKER_P_H
#define LVFILTERWORKER_P_H

#include <QObject>
#include <QCoreApplication>
#include "live/workerthread.h"
#include "qmlstreamfilter.h"

namespace lv{

/// \private
class WorkerThreadPrivate : public QObject{

    Q_OBJECT

public:
    explicit WorkerThreadPrivate(WorkerThread* worker) : QObject(nullptr), m_worker(worker){}
    ~WorkerThreadPrivate(){}

    void postNotify(WorkerThread::CallEvent* evt){
        QCoreApplication::postEvent(this, evt);
    }

    bool event(QEvent *event){
        if (!dynamic_cast<WorkerThread::CallEvent*>(event))
            return QObject::event(event);

        WorkerThread::CallEvent* ce = static_cast<WorkerThread::CallEvent*>(event);

        QObject* caller = m_worker->acts()[ce->m_callerIndex];
        QmlAct* act = qobject_cast<QmlAct*>(caller);
        if ( act ){
            act->setResult(ce->m_args);
        } else {
//            QmlStreamFilter* sf = qobject_cast<QmlStreamFilter*>(caller);
//            if( sf ){
//                sf->pushResult(ce->m_args);
//            }
        }

        m_worker->m_isWorking = false;
        m_worker->postNextInProcessQueue();

        return true;
    }

private:
    WorkerThread* m_worker;

};

}// namespace

#endif // LVFILTERWORKER_P_H
