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

#include "filterworker.h"
#include "filterworker_p.h"
#include <QThread>
#include <QCoreApplication>

namespace lv{

FilterWorker::FilterWorker(QObject *)
    : QObject(0)
    , m_thread(new QThread)
    , m_d(new FilterWorkerPrivate)
{
    moveToThread(m_thread);
}

FilterWorker::~FilterWorker(){
    m_thread->deleteLater();
    //TODO: Handle results after thread finishes
    delete m_d;
}

void FilterWorker::postWork(const std::function<void ()> &fnc) {
    QCoreApplication::postEvent(this, new FilterWorker::CallEvent(fnc));
}

void FilterWorker::postWork(const std::function<void ()> &fnc, const std::function<void ()> &cbk){
    QCoreApplication::postEvent(this, new FilterWorker::CallEvent(fnc, cbk));
}

void FilterWorker::start(){
    m_thread->start();
}

bool FilterWorker::event(QEvent *ev){
    if (!dynamic_cast<FilterWorker::CallEvent*>(ev))
        return QObject::event(ev);

    FilterWorker::CallEvent* ce = static_cast<FilterWorker::CallEvent*>(ev);
    ce->callFilter();

    if ( ce->hasCallback() ){
        m_d->postNotify(ce->callbackEvent());
    }

    return true;
}

FilterWorker::CallEvent::CallEvent(const std::function<void ()>& fnc)
    : QEvent(QEvent::None)
    , m_filter(fnc)
{
}

FilterWorker::CallEvent::CallEvent(std::function<void ()>&& fnc)
    : QEvent(QEvent::None)
    , m_filter(std::move(fnc))
{
}

FilterWorker::CallEvent::CallEvent(const std::function<void ()> &filter, const std::function<void ()> &callback)
    : QEvent(QEvent::None)
    , m_filter(filter)
    , m_callback(callback)
{
}

FilterWorker::CallEvent::CallEvent(std::function<void ()> &&filter, std::function<void ()> &&callback)
    : QEvent(QEvent::None)
    , m_filter(filter)
    , m_callback(callback)
{
}

void FilterWorker::CallEvent::callFilter(){
    m_filter();
}

bool FilterWorker::CallEvent::hasCallback(){
    return m_callback ? true : false;
}

FilterWorker::CallEvent *FilterWorker::CallEvent::callbackEvent(){
    return new FilterWorker::CallEvent(m_callback);
}

}// namespace
