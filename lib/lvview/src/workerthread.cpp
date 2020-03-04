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

#include "workerthread.h"
#include "workerthread_p.h"
#include "live/visuallogqt.h"
#include "qmlstreamfilter.h"

#include <QtDebug>

#include <QThread>
#include <QCoreApplication>

namespace lv{

WorkerThread::WorkerThread(const QList<QString>& actSources, QObject *)
    : QObject(nullptr)
    , m_isWorking(false)
    , m_engine(nullptr)
    , m_thread(new QThread)
    , m_actFunctionsSource(actSources)
    , m_d(new WorkerThreadPrivate(this))
{
    moveToThread(m_thread);
}

WorkerThread::~WorkerThread(){
    m_thread->exit();
    if ( !m_thread->wait(1500) ){
        qCritical("FilterWorker Thread failed to close, forcing quit. This may lead to inconsistent application state.");
        m_thread->terminate();
        m_thread->wait();
    }
    delete m_thread;
    delete m_engine;
    delete m_d;
}

void WorkerThread::postWork(QmlStreamFilter *caller, const QVariant &value, const QList<Shared *> objectTransfers){
    if ( m_isWorking ){
        m_toExecute.removeOne(caller);
        m_toExecute.prepend(caller);
        return;
    }

    m_isWorking = true;

    for ( Shared* obj : objectTransfers ){
        obj->moveToThread(m_thread);
    }

    int index = m_calls.indexOf(caller);

    QCoreApplication::postEvent(this, new WorkerThread::CallEvent(index, QVariantList() << value));
}

void WorkerThread::postWork(QmlAct *caller, const QVariantList &values, const QList<Shared *> objectTransfers){
    if ( m_isWorking ){
        m_toExecute.removeOne(caller);
        m_toExecute.prepend(caller);
        return;
    }

    m_isWorking = true;

    for ( Shared* obj : objectTransfers ){
        obj->moveToThread(m_thread);
    }

    int index = m_calls.indexOf(caller);

    if ( caller->run().isArray() ){
        QJSValue obj = caller->run().property(0);
        QObject* o = obj.toVariant().value<QObject*>();

        QObject* oclone = o->metaObject()->newInstance();
        oclone->moveToThread(m_thread);
        oclone->setParent(m_engine);

        m_specialFunctions.insert(index, qMakePair(oclone, caller->run().property(1).toString()));
    }

    QCoreApplication::postEvent(this, new WorkerThread::CallEvent(index, values));
}

void WorkerThread::start(){
    m_thread->start();
}

bool WorkerThread::isWorking() const{
    return m_isWorking;
}

bool WorkerThread::event(QEvent *ev){
    if (!dynamic_cast<WorkerThread::CallEvent*>(ev))
        return QObject::event(ev);

    if ( !m_engine ){
        m_engine = new QJSEngine;
        m_engine->installExtensions(QJSEngine::ConsoleExtension);
        for ( const QString& src : m_actFunctionsSource ){
            QJSValue c = m_engine->evaluate(src);
            m_actFunctions.append(c);
        }
        for ( auto it = m_specialFunctions.begin(); it != m_specialFunctions.end(); ++it ){
            int index    = it.key();
            QObject* o   = it.value().first;
            QString prop = it.value().second;

            QString objectName = "F" + QString::number(index);

            m_engine->globalObject().setProperty(objectName, m_engine->newQObject(o));
            QJSValue c = m_engine->evaluate(objectName + "." + prop);
            m_actFunctions[index] = c;
        }
    }

    WorkerThread::CallEvent* ce = static_cast<WorkerThread::CallEvent*>(ev);
    QVariantList values = ce->m_args.toList();

    QJSValueList args;
    for ( const QVariant& v : values ){
        args.append(Shared::transfer(v, m_engine));
    }

    QJSValue r = m_actFunctions[ce->m_callerIndex].call(args);

    QList<Shared*> robj;
    QVariant rv = Shared::transfer(r, robj);

    QObject* a = m_calls.at(ce->m_callerIndex);
    for ( Shared* sh : robj ){
        QObject* o = static_cast<QObject*>(sh);
        o->moveToThread(a->thread());
    }

    m_d->postNotify(new WorkerThread::CallEvent(ce->m_callerIndex, rv));

    return true;
}

void WorkerThread::postNextInProcessQueue(){
    if ( !m_toExecute.isEmpty() ){
        QObject* caller = m_toExecute.front();
        m_toExecute.pop_front();

        QmlAct* act = qobject_cast<QmlAct*>(caller);
        if ( act ){
            act->exec();
        } else {
            QmlStreamFilter* sf = qobject_cast<QmlStreamFilter*>(caller);
            if( sf ){
                sf->triggerRun();
            }
        }
    }
}

WorkerThread::CallEvent::CallEvent(int callerIndex, const QVariant &args, const QList<Shared *> &transferObjects)
    : QEvent(QEvent::None)
    , m_callerIndex(callerIndex)
    , m_args(args)
    , m_transferObjects(transferObjects)
{
}

}// namespace
