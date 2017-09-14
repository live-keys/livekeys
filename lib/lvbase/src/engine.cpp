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

#include "live/engine.h"
#include "live/exception.h"
#include "live/errorhandler.h"
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
    m_engine->setOutputWarningsToStandardError(false);
    m_errorType = m_engine->evaluate("Error");
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

QJSValue Engine::evaluate(const QString &jsCode, const QString &fileName, int lineNumber){
    return m_engine->evaluate(jsCode, fileName, lineNumber);
}

void Engine::throwError(const lcv::Exception *e, QObject *object){
    QJSValue jsError = m_errorType.callAsConstructor(QJSValueList() << e->message());
    jsError.setProperty("code", e->code());

    if ( e->hasLocation() ){
        jsError.setProperty("fileName", e->file());
        jsError.setProperty("lineNumber", e->line());
        jsError.setProperty("functionName", e->functionName());
    }

    if ( object ){
        jsError.setProperty("object", m_engine->newQObject(object));
    }

    if ( e->hasStackTrace() ){
        StackTrace::Ptr st = e->stackTrace();
        QJSValue stackTrace = m_engine->newArray(static_cast<int>(st->size()));
        int i = 0;
        for ( auto it = st->begin(); it != st->end(); ++it ){
            stackTrace.setProperty(i++, QString::fromStdString(it->functionName()) + "(" + it->fileName().c_str() + ":" + QString::number(it->line()) + ")");
        }
        jsError.setProperty("stack", stackTrace);
    }

    if ( dynamic_cast<const lcv::FatalException*>(e) != nullptr ){
        jsError.setProperty("type", "FatalException");
    } else if ( dynamic_cast<const lcv::InputException*>(e) != nullptr ){
        jsError.setProperty("type", "ConfigurationException");
    } else {
        jsError.setProperty("type", "Exception");
    }

    throwError(jsError, object);
}


void Engine::throwError(const QQmlError &error){
    QJSValue jsError = m_errorType.callAsConstructor(QJSValueList() << error.description());
    jsError.setProperty("fileName", error.url().toString());
    jsError.setProperty("lineNumber", error.line());
    jsError.setProperty("type", "Error");
    if ( error.object() ){
        jsError.setProperty("object", m_engine->newQObject(error.object()));
    }

    throwError(jsError, error.object());
}

void Engine::throwError(const QJSValue &jsError, QObject *object){
    QObject* errorHandler = object;
    while ( errorHandler != 0 ){
        auto it = m_errorHandlers.find(object);
        if ( it != m_errorHandlers.end() ){
            it.value()->signalError(jsError);
            return;
        }
        errorHandler = object->parent();
    }

    emit applicationError(jsError);

}

void Engine::throwWarning(const QString &message, QObject *object, const QString &fileName, int lineNumber){
    QJSValue jsError = m_errorType.callAsConstructor(QJSValueList() << message);

    jsError.setProperty("fileName", fileName);
    jsError.setProperty("lineNumber", lineNumber);
    jsError.setProperty("type", "Warning");
    if ( object ){
        jsError.setProperty("object", m_engine->newQObject(object));
    }

    throwWarning(jsError, object);
}

void Engine::throwWarning(const QJSValue &jsError, QObject *object){
    QObject* errorHandler = object;
    while ( errorHandler != 0 ){
        auto it = m_errorHandlers.find(object);
        if ( it != m_errorHandlers.end() ){
            it.value()->signalWarning(jsError);
            return;
        }
        errorHandler = object->parent();
    }

    emit applicationWarning(jsError);
}

bool Engine::hasErrorHandler(QObject *object){
    return m_errorHandlers.contains(object);
}

void Engine::registerErrorHandler(QObject *object, ErrorHandler *handler){
    m_errorHandlers[object] = handler;
}

void Engine::removeErrorHandler(QObject *object){
    m_errorHandlers.remove(object);
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
