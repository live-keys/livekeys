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

#include "workerthread.h"
#include "workerthread_p.h"
#include <QThread>
#include <QCoreApplication>

namespace lv{

WorkerThread::WorkerThread(QObject *)
    : QObject(0)
    , m_thread(new QThread)
    , m_d(new WorkerThreadPrivate)
{
    moveToThread(m_thread);
}

WorkerThread::~WorkerThread(){
    m_thread->exit();
    if ( !m_thread->wait(5000) ){
        qCritical("FilterWorker Thread failed to close, forcing quit. This may lead to inconsistent application state.");
        m_thread->terminate();
        m_thread->wait();
    }
    delete m_thread;
    delete m_d;
}

void WorkerThread::postWork(
        const std::function<void ()> &fnc,
        Shared::ReadScope *locker)
{
    QCoreApplication::postEvent(this, new WorkerThread::CallEvent(fnc, locker));
}

void WorkerThread::postWork(
        const std::function<void ()> &fnc,
        const std::function<void ()> &cbk,
        Shared::ReadScope *locker)
{
    QCoreApplication::postEvent(this, new WorkerThread::CallEvent(fnc, cbk, locker));
}

void WorkerThread::start(){
    m_thread->start();
}

bool WorkerThread::event(QEvent *ev){
    if (!dynamic_cast<WorkerThread::CallEvent*>(ev))
        return QObject::event(ev);

    WorkerThread::CallEvent* ce = static_cast<WorkerThread::CallEvent*>(ev);
        ce->callFilter();

    if ( ce->hasCallback() ){
        // locker will be deleted in the callback
        m_d->postNotify(ce->callbackEvent());
    } else {
        // locker can be deleted now
        delete ce->readScope();
    }

    return true;
}

WorkerThread::CallEvent::CallEvent(const std::function<void ()>& fnc, Shared::ReadScope *locker)
    : QEvent(QEvent::None)
    , m_filter(fnc)
    , m_readScope(locker)
{
}

WorkerThread::CallEvent::CallEvent(std::function<void ()>&& fnc, Shared::ReadScope *locker)
    : QEvent(QEvent::None)
    , m_filter(std::move(fnc))
    , m_readScope(locker)
{
}

WorkerThread::CallEvent::CallEvent(
        const std::function<void ()> &filter,
        const std::function<void ()> &callback,
        Shared::ReadScope *locker)
    : QEvent(QEvent::None)
    , m_filter(filter)
    , m_callback(callback)
    , m_readScope(locker)
{
}

WorkerThread::CallEvent::CallEvent(
        std::function<void ()> &&filter,
        std::function<void ()> &&callback,
        Shared::ReadScope *locker)
    : QEvent(QEvent::None)
    , m_filter(filter)
    , m_callback(callback)
    , m_readScope(locker)
{
}

void WorkerThread::CallEvent::callFilter(){
    m_filter();
}

Shared::ReadScope *WorkerThread::CallEvent::readScope(){
    return m_readScope;
}

bool WorkerThread::CallEvent::hasCallback(){
    return m_callback ? true : false;
}

WorkerThread::CallEvent *WorkerThread::CallEvent::callbackEvent(){
    return new WorkerThread::CallEvent(m_callback, m_readScope);
}

}// namespace
