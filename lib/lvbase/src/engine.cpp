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

#include "Engine.h"
#include "live/incubationcontroller.h"

#include <QQmlComponent>
#include <QQmlIncubator>
#include <QQmlEngine>
#include <QQuickItem>
#include <QQmlIncubationController>
#include <QMutex>
#include <QMutexLocker>

#include <QCoreApplication>

#include <QDebug>

#include <QQuickItem>

namespace lcv{

Engine::Engine(QQmlEngine *engine, QObject *parent)
    : QObject(parent)
    , m_engine(engine)
    , m_engineMutex(new QMutex)
    , m_incubator(new QQmlIncubator(QQmlIncubator::Asynchronous))
    , m_incubationController(new IncubationController)
{
    m_engine->setIncubationController(m_incubationController);
}

Engine::~Engine(){
    delete m_engineMutex;
    m_engine->deleteLater();
}

void Engine::useEngine(std::function<void(QQmlEngine *)> call){
    QMutexLocker engineMutexLock(m_engineMutex);
    call(m_engine);
}

const QList<QQmlError> &Engine::lastErrors() const{
    return m_lastErrors;
}

void Engine::createObjectAsync(
        const QString& qmlCode,
        QObject* parent,
        const QUrl& url,
        bool clearCache)
{
    m_engineMutex->lock();

    emit aboutToCreateObject(url);

    if ( clearCache )
        m_engine->clearComponentCache();

    QQmlComponent component(m_engine);
    component.setData(qmlCode.toUtf8(), url);

    QList<QQmlError> errors = component.errors();
    if ( errors.size() > 0 ){
        m_engineMutex->unlock();
        emit objectCreationError(toJSErrors(errors));
        return;
    }

    QQmlIncubator incubator;
    component.create(incubator, m_engine->rootContext());
    setIsLoading(true);

    while (incubator.isLoading()) {
        QCoreApplication::processEvents(QEventLoop::AllEvents, 15);
    }

    QList<QQmlError> incubatorErrors = component.errors();
    if ( incubatorErrors.size() > 0 ){
        setIsLoading(false);
        QJSValue jsErrors = toJSErrors(incubatorErrors);
        m_engineMutex->unlock();
        emit objectCreationError(jsErrors);
        return;
    }

    if ( incubator.isNull() ){
        setIsLoading(false);
        QQmlError errorObject;
        errorObject.setDescription("Component returned null object.");
        QJSValue jsErrors = toJSErrors(QList<QQmlError>() << errorObject);
        m_engineMutex->unlock();
        emit objectCreationError(jsErrors);
        return;
    }

    QObject* obj = incubator.object();
    m_engine->setObjectOwnership(obj, QQmlEngine::JavaScriptOwnership);

    if (parent)
        obj->setParent(parent);

    QQuickItem *parentItem = qobject_cast<QQuickItem*>(parent);
    QQuickItem *item = qobject_cast<QQuickItem*>(obj);
    if (parentItem && item){
        item->setParentItem(parentItem);
    }

    //TODO
//    QDocumentQmlInfo::syncBindings(qmlCode, url, obj);

    setIsLoading(false);

    m_engineMutex->unlock();
    emit objectCreated(obj);
}

QJSValue Engine::lastErrorsObject() const{
    return toJSErrors(lastErrors());
}

QObject* Engine::createObject(const QString &qmlCode, QObject *parent, const QUrl &url, bool clearCache){
    QMutexLocker engineMutexLock(m_engineMutex);

    if ( clearCache )
        m_engine->clearComponentCache();

    QQmlComponent component(m_engine);
    component.setData(qmlCode.toUtf8(), url);

    m_lastErrors = component.errors();
    if ( m_lastErrors.size() > 0 ){
        return 0;
    }

    QObject* obj = component.create(m_engine->rootContext());
    m_engine->setObjectOwnership(obj, QQmlEngine::JavaScriptOwnership);

    m_lastErrors = component.errors();
    if ( m_lastErrors.size() > 0 ){
        return 0;
    }

    if (parent)
        obj->setParent(parent);

    QQuickItem *parentItem = qobject_cast<QQuickItem*>(parent);
    QQuickItem *item = qobject_cast<QQuickItem*>(obj);
    if (parentItem && item){
        item->setParentItem(parentItem);
    }

    return obj;
}

QJSValue Engine::toJSErrors(const QList<QQmlError> &errors) const{
    QJSValue val = m_engine->newArray(errors.length());
    int i = 0;
    foreach( const QQmlError& error, errors ){
        QJSValue qmlErrOjbect = m_engine->newObject();
        qmlErrOjbect.setProperty("lineNumber", QJSValue(error.line()));
        qmlErrOjbect.setProperty("columnNumber", QJSValue(error.column()));
        qmlErrOjbect.setProperty("fileName", QJSValue(error.url().toString()));
        qmlErrOjbect.setProperty("message", QJSValue(error.description()));
        val.setProperty(i++, qmlErrOjbect);
    }
    return val;
}

}// namespace
