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
#include <QJSValueIterator>

#include <QCoreApplication>

#include <QQuickItem>

namespace lv{

Engine::Engine(QQmlEngine *engine, QObject *parent)
    : QObject(parent)
    , m_engine(engine)
    , m_engineMutex(new QMutex)
    , m_incubator(new QQmlIncubator(QQmlIncubator::Asynchronous))
    , m_incubationController(new IncubationController)
{
    m_engine->setIncubationController(m_incubationController);
    m_engine->setOutputWarningsToStandardError(true);
    connect(m_engine, SIGNAL(warnings(QList<QQmlError>)), this, SLOT(engineWarnings(QList<QQmlError>)));
    m_errorType = m_engine->evaluate("Error");

    QJSValue markErrorConstructor = m_engine->evaluate(
        "function(engine){"
            "return function(error, object){\n"
                "error.message += engine.markErrorObject(object);"
                "return error;"
            "}"
        "}"
    );
    QJSValue markErrorFn = markErrorConstructor.call(QJSValueList() << engine->newQObject(this));
    m_engine->globalObject().setProperty("linkedError", markErrorFn);
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

void Engine::throwError(const lv::Exception *e, QObject *object){
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

    if ( dynamic_cast<const lv::FatalException*>(e) != nullptr ){
        jsError.setProperty("type", "FatalException");
    } else if ( dynamic_cast<const lv::InputException*>(e) != nullptr ){
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
    QJSValueIterator it(jsError);
    while (it.hasNext()) {
        it.next();
    }
    QObject* errorHandler = object;
    while ( errorHandler != 0 ){
        auto it = m_errorHandlers.find(errorHandler);
        if ( it != m_errorHandlers.end() ){
            it.value()->signalError(jsError);
            return;
        }
        errorHandler = errorHandler->parent();
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
        auto it = m_errorHandlers.find(errorHandler);
        if ( it != m_errorHandlers.end() ){
            it.value()->signalWarning(jsError);
            return;
        }
        errorHandler = errorHandler->parent();
    }

    emit applicationWarning(jsError);
}

QString Engine::markErrorObject(QObject *object){
    QString key = "0x" + QString::number((quintptr)object, 16);
    m_errorObjects[key] = object;

    return "(" + key + ")";
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

void Engine::setBindHook(std::function<void(const QString&, const QUrl&, QObject*, QObject*)> hook){
    m_bindHook = hook;
}


TypeInfo::Ptr Engine::typeInfo(const QMetaObject *key) const{
    auto it = m_types.find(key);
    if ( it == m_types.end() )
        return TypeInfo::Ptr(0);
    return it.value();
}

TypeInfo::Ptr Engine::typeInfo(const QByteArray &typeName) const{
    auto it = m_typeNames.find(typeName);
    if ( it == m_typeNames.end() )
        return TypeInfo::Ptr(0);

    return m_types[*it];
}

TypeInfo::Ptr Engine::typeInfo(const QMetaType &metaType) const{
    const QMetaObject* mo = metaType.metaObject();
    if ( !mo )
        return TypeInfo::Ptr(0);

    return typeInfo(mo);
}

void Engine::createObjectAsync(
        const QString& qmlCode,
        QObject* parent,
        const QUrl& url,
        QObject* attachment,
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


    if ( m_bindHook )
        m_bindHook(qmlCode, url, obj, attachment);

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

void Engine::engineWarnings(const QList<QQmlError> &warnings){
    for ( auto it = warnings.begin(); it != warnings.end(); ++it ){
        const QQmlError& warning = *it;
        if ( warning.object() == 0 ){
            QString description = warning.description();
            int errorObjectEnd = description.lastIndexOf(")");
            if ( errorObjectEnd == description.length() - 1 ){
                int errorObjectStart = description.lastIndexOf("(0x");
                if ( errorObjectStart != -1 ){
                    QString key = description.mid(errorObjectStart + 1, errorObjectEnd - errorObjectStart - 1);
                    auto it = m_errorObjects.find(key);
                    if ( it != m_errorObjects.end() ){
                        QQmlError err = warning;
                        err.setDescription(description.mid(0, errorObjectStart));
                        err.setObject(it.value());
                        throwError(err);
                        return;
                    }
                }
            }
        }

        throwError(*it);
    }
    m_errorObjects.clear();
}

QJSValue Engine::toJSError(const QQmlError &error) const{
    QJSValue qmlErrOjbect = m_engine->newObject();
    qmlErrOjbect.setProperty("lineNumber", QJSValue(error.line()));
    qmlErrOjbect.setProperty("columnNumber", QJSValue(error.column()));
    qmlErrOjbect.setProperty("fileName", QJSValue(error.url().toString()));
    qmlErrOjbect.setProperty("message", QJSValue(error.description()));
    return qmlErrOjbect;
}

QJSValue Engine::toJSErrors(const QList<QQmlError> &errors) const{
    QJSValue val = m_engine->newArray(errors.length());
    int i = 0;
    foreach( const QQmlError& error, errors ){
        val.setProperty(i++, toJSError(error));
    }
    return val;
}

}// namespace
