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

#include "live/viewengine.h"
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


/**
 * \class lv::FatalException
 * \brief Subclass of the lv::Exception used for unrecoverable errors
 *
 *
 * \ingroup lvview
 */


/**
 * \class lv::InputException
 * \brief Subclass of the lv::Exception used for configuration errors
 * \ingroup lvview
 */


/**
 * \class lv::ViewEngine
 * \brief Main Qml engine
 * \ingroup lvview
 */

namespace lv{


/** Default constructor */
ViewEngine::ViewEngine(QQmlEngine *engine, QObject *parent)
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
        "(function(engine){"
            "return function(error, object){\n"
                "error.message += engine.markErrorObject(object);"
                "return error;"
            "}"
        "})"
    );
    QJSValue markErrorFn = markErrorConstructor.call(QJSValueList() << engine->newQObject(this));
    m_engine->globalObject().setProperty("linkError", markErrorFn);
}

/** Default destructor */
ViewEngine::~ViewEngine(){
    delete m_engineMutex;
    m_engine->deleteLater();
}

/** Locks the engine for use until the passed function finishes */
void ViewEngine::useEngine(std::function<void(QQmlEngine *)> call){
    QMutexLocker engineMutexLock(m_engineMutex);
    call(m_engine);
}

/** Displays the errors the engine had previously */
const QList<QQmlError> &ViewEngine::lastErrors() const{
    return m_lastErrors;
}

/** Evaluates the piece of code given the filename and line number */
QJSValue ViewEngine::evaluate(const QString &jsCode, const QString &fileName, int lineNumber){
    return m_engine->evaluate(jsCode, fileName, lineNumber);
}

/**
 * \brief Function analogue to throwing an exception in regular cpp code, but propagated through javascript
 *
 * We use a simple macro, `CREATE_EXCEPTION`, to create exceptions in LiveKeys, which will include the metadata of the code where it was thrown.
 * Usage example of this function can be found
 * in qcalibratedebevec.cpp.
 *
 * ```
 * lv::Exception lve = CREATE_EXCEPTION(lv::Exception, e.what(), e.code);
 * lv::ViewContext::instance().engine()->throwError(&lve, this);
 * ```
 *
 * Calling the throwError manages to capture all the relevant data such as line number, type of error, and even the object from
 * which it was thrown, and also the stacktrace containing even the relevant Javascript info. The error is propagated until we
 * reach a relevant error handler (\sa lv::ErrorHandler).
 */

void ViewEngine::throwError(const lv::Exception *e, QObject *object){
    QJSValue jsError = m_errorType.callAsConstructor(QJSValueList() << QString::fromStdString(e->message()));
    jsError.setProperty("code", static_cast<double>(e->code()));

    if ( e->hasLocation() ){
        jsError.setProperty("fileName", QString::fromStdString(e->file()));
        jsError.setProperty("lineNumber", e->line());
        jsError.setProperty("functionName", QString::fromStdString(e->functionName()));
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
        jsError.setProperty("stackTrace", stackTrace);
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

/** Variant of the same-named function that uses a QQmlError object from which it extracts relevant data */
void ViewEngine::throwError(const QQmlError &error){
    QJSValue jsError = m_errorType.callAsConstructor(QJSValueList() << error.description());
    jsError.setProperty("fileName", error.url().toString());
    jsError.setProperty("lineNumber", error.line());
    jsError.setProperty("type", "Error");
    if ( error.object() ){
        jsError.setProperty("object", m_engine->newQObject(error.object()));
    }

    throwError(jsError, error.object());
}

/**
 * \brief Variant of the same-named function that uses a QJSValue object along with the calling object
 *
 * This is the actual function that gets called from the two variants above. The relevant data is converted to
 * QJSValue and passed to here.
 */
void ViewEngine::throwError(const QJSValue &jsError, QObject *object){
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

/** Similar to throwError, but warnings are of lesser importance and can be ignored. Passed to the error handler */
void ViewEngine::throwWarning(const QString &message, QObject *object, const QString &fileName, int lineNumber){
    QJSValue jsError = m_errorType.callAsConstructor(QJSValueList() << message);

    jsError.setProperty("fileName", fileName);
    jsError.setProperty("lineNumber", lineNumber);
    jsError.setProperty("type", "Warning");
    if ( object ){
        jsError.setProperty("object", m_engine->newQObject(object));
    }

    throwWarning(jsError, object);
}

/** The function called by the same-named public function. Passes the warning to the error handler(s) */
void ViewEngine::throwWarning(const QJSValue &jsError, QObject *object){
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

QString ViewEngine::markErrorObject(QObject *object){
    QString key = "0x" + QString::number((quintptr)object, 16);
    m_errorObjects[key] = object;

    return "(" + key + ")";
}

/** Shows if the given object has an associated error handler */
bool ViewEngine::hasErrorHandler(QObject *object){
    return m_errorHandlers.contains(object);
}

/** Registers a handler for the given object */
void ViewEngine::registerErrorHandler(QObject *object, ErrorHandler *handler){
    m_errorHandlers[object] = handler;
}

/** Removes the handler for the given object */
void ViewEngine::removeErrorHandler(QObject *object){
    m_errorHandlers.remove(object);
}

/** Added after the compilation is finished, to be run as a callback */
void ViewEngine::addCompileHook(ViewEngine::CompileHook ch, void *userData){
    CompileHookEntry che;
    che.m_hook = ch;
    che.m_userData = userData;

    m_compileHooks.append(che);
}

/** Removes the given compile hook */
void ViewEngine::removeCompileHook(ViewEngine::CompileHook ch, void *userData){
    for ( auto it = m_compileHooks.begin(); it != m_compileHooks.end(); ++it ){
        if ( it->m_hook == ch && it->m_userData == userData ){
            m_compileHooks.erase(it);
            return;
        }
    }
}

/** Returns the type info for a given meta-object*/
TypeInfo::Ptr ViewEngine::typeInfo(const QMetaObject *key) const{
    auto it = m_types.find(key);
    if ( it == m_types.end() )
        return TypeInfo::Ptr(0);
    return it.value();
}

/** Returns the type info for a given type name */
TypeInfo::Ptr ViewEngine::typeInfo(const QByteArray &typeName) const{
    auto it = m_typeNames.find(typeName);
    if ( it == m_typeNames.end() )
        return TypeInfo::Ptr(0);

    return m_types[*it];
}

/** Returns the type info for a given meta-type by extracting the meta-object and calling the appropriate variant of the getter */
TypeInfo::Ptr ViewEngine::typeInfo(const QMetaType &metaType) const{
    const QMetaObject* mo = metaType.metaObject();
    if ( !mo )
        return TypeInfo::Ptr(0);

    return typeInfo(mo);
}

/**
 * \brief Generates a message for uncreatable types that are available as properties
 */
QString ViewEngine::typeAsPropertyMessage(const QString &typeName, const QString &propertyName){
    return typeName + " is available by accessing the \'" + propertyName + "\' property.";
}

/**
 * \brief Main function of the engine, where we pass the code to be compiled asynchronously
 *
 * Of extreme importance to the live coding part of LiveKeys, since we use it to compile our custom code.
 * clearCache is used to indicate that we've changed one of the non-active files which are cached, so the
 * cache has to be cleared since it's invalid after a change.
 */
void ViewEngine::createObjectAsync(
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

    for (auto it = m_compileHooks.begin(); it != m_compileHooks.end(); ++it)
        (it->m_hook)(qmlCode, url, obj, attachment, it->m_userData);

    setIsLoading(false);

    m_engineMutex->unlock();
    emit objectCreated(obj);
}

QJSValue ViewEngine::lastErrorsObject() const{
    return toJSErrors(lastErrors());
}

/** Synchronous variant of the same creation of object to be used for compilation of our code */
QObject* ViewEngine::createObject(const QString &qmlCode, QObject *parent, const QUrl &url, bool clearCache){
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

/** Throws errors on these warnings which don't have their own object  */
void ViewEngine::engineWarnings(const QList<QQmlError> &warnings){
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

QJSValue ViewEngine::toJSError(const QQmlError &error) const{
    QJSValue qmlErrOjbect = m_engine->newObject();
    qmlErrOjbect.setProperty("lineNumber", QJSValue(error.line()));
    qmlErrOjbect.setProperty("columnNumber", QJSValue(error.column()));
    qmlErrOjbect.setProperty("fileName", QJSValue(error.url().toString()));
    qmlErrOjbect.setProperty("message", QJSValue(error.description()));
    return qmlErrOjbect;
}

QJSValue ViewEngine::toJSErrors(const QList<QQmlError> &errors) const{
    QJSValue val = m_engine->newArray(errors.length());
    int i = 0;
    foreach( const QQmlError& error, errors ){
        val.setProperty(i++, toJSError(error));
    }
    return val;
}

}// namespace
