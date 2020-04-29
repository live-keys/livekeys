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

#include "live/viewengine.h"
#include "live/exception.h"
#include "live/errorhandler.h"
#include "live/incubationcontroller.h"
#include "live/packagegraph.h"
#include "live/applicationcontext.h"

#include "qmlcontainer.h"
#include "qmlact.h"
#include "qmlact.h"
#include "qmlfollowup.h"
#include "qmlopening.h"
#include "group.h"
#include "groupcollector.h"
#include "layer.h"
#include "windowlayer.h"
#include "qmlstream.h"
#include "qmlwritablestream.h"
#include "qmlstreamfilter.h"
#include "qmlstreamiterator.h"
#include "qmlclipboard.h"
#include "live/settings.h"
#include "live/memory.h"
#include "live/visuallogmodel.h"
#include "live/visuallogqmlobject.h"

#include <QQmlComponent>
#include <QQmlContext>
#include <QQmlIncubator>
#include <QQmlEngine>
#include <QQuickItem>
#include <QQmlIncubationController>
#include <QMutex>
#include <QMutexLocker>
#include <QJSValueIterator>

#include <QCoreApplication>

#include <QQuickItem>

#include "qmlvariantlist.h"
#include "qmlvariantlistmodel.h"
#include "qmlobjectlist.h"
#include "qmlobjectlistmodel.h"

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
    , m_packageGraph(nullptr)
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

//    connect(m_engine, &QQmlEngine::quit, QCoreApplication::instance(),
//                      &QCoreApplication::quit, Qt::QueuedConnection);

//#if (QT_VERSION > QT_VERSION_CHECK(5,8,0))
//    connect(m_engine, &QQmlEngine::exit, QCoreApplication::instance(),
//                      &QCoreApplication::exit, Qt::QueuedConnection);
//#endif
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
        QJSValue stackTrace = m_engine->newArray(static_cast<quint32>(st->size()));
        quint32 i = 0;
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
    while ( errorHandler != nullptr ){
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
    while ( errorHandler != nullptr ){
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
MetaInfo::Ptr ViewEngine::typeInfo(const QMetaObject *key) const{
    auto it = m_types.find(key);
    if ( it == m_types.end() )
        return MetaInfo::Ptr(nullptr);
    return it.value();
}

/** Returns the type info for a given type name */
MetaInfo::Ptr ViewEngine::typeInfo(const QByteArray &typeName) const{
    auto it = m_typeNames.find(typeName);
    if ( it == m_typeNames.end() )
        return MetaInfo::Ptr(nullptr);

    return m_types[*it];
}

/** Returns the type info for a given meta-type by extracting the meta-object and calling the appropriate variant of the getter */
MetaInfo::Ptr ViewEngine::typeInfo(const QMetaType &metaType) const{
    const QMetaObject* mo = metaType.metaObject();
    if ( !mo )
        return MetaInfo::Ptr(nullptr);

    return typeInfo(mo);
}

/**
 * \brief Generates a message for uncreatable types that are available as properties
 */
QString ViewEngine::typeAsPropertyMessage(const QString &typeName, const QString &propertyName){
    return typeName + " is available by accessing the \'" + propertyName + "\' property.";
}

/**
 * \brief Register the base types from the view library
 */
void ViewEngine::registerBaseTypes(const char *uri){
    qmlRegisterType<lv::ErrorHandler>(          uri, 1, 0, "ErrorHandler");
    qmlRegisterType<lv::QmlContainer>(          uri, 1, 0, "Container");
    qmlRegisterType<lv::QmlAct>(                uri, 1, 0, "Act");
    qmlRegisterType<lv::QmlOpening>(            uri, 1, 0, "Opening");
    qmlRegisterType<lv::QmlFollowUp>(           uri, 1, 0, "FollowUp");
    qmlRegisterType<lv::Group>(                 uri, 1, 0, "Group");
    qmlRegisterType<lv::GroupCollector>(        uri, 1, 0, "GroupCollector");
    qmlRegisterType<lv::QmlVariantList>(        uri, 1, 0, "VariantList");
    qmlRegisterType<lv::QmlObjectList>(         uri, 1, 0, "ObjectList");
    qmlRegisterType<lv::QmlVariantListModel>(   uri, 1, 0, "VariantListModel");
    qmlRegisterType<lv::QmlObjectListModel>(    uri, 1, 0, "ObjectListModel");
    qmlRegisterType<lv::WindowLayer>(           uri, 1, 0, "WindowLayer");
    qmlRegisterType<lv::QmlClipboard>(          uri, 1, 0, "Clipboard");
    qmlRegisterType<lv::QmlStream>(             uri, 1, 0, "Stream");
    qmlRegisterType<lv::QmlStreamFilter>(       uri, 1, 0, "StreamFilter");
    qmlRegisterType<lv::QmlWritableStream>(     uri, 1, 0, "WritableStream");
    qmlRegisterType<lv::QmlStreamIterator>(     uri, 1, 0, "StreamIterator");

    qmlRegisterUncreatableType<lv::Shared>(     uri, 1, 0, "Shared", "Shared is of abstract type.");
    qmlRegisterUncreatableType<lv::Layer>(      uri, 1, 0, "Layer", "Layer is of abstract type.");

    qmlRegisterUncreatableType<lv::ViewEngine>(
        uri, 1, 0, "LiveEngine",      ViewEngine::typeAsPropertyMessage("LiveEngine", "lk.engine"));
    qmlRegisterUncreatableType<lv::Settings>(
        uri, 1, 0, "LiveSettings",    ViewEngine::typeAsPropertyMessage("LiveSettings", "lk.settings"));
    qmlRegisterUncreatableType<lv::VisualLogModel>(
        uri, 1, 0, "VisualLogModel",  ViewEngine::typeAsPropertyMessage("VisualLogModel", "lk.log"));
    qmlRegisterUncreatableType<lv::Memory>(
        uri, 1, 0, "Memory",          ViewEngine::typeAsPropertyMessage("Memory", "lk.mem"));
    qmlRegisterUncreatableType<lv::VisualLogQmlObject>(
        uri, 1, 0, "VisualLog",       ViewEngine::typeAsPropertyMessage("VisualLog", "vlog"));
    qmlRegisterUncreatableType<lv::VisualLogBaseModel>(
        uri, 1, 0, "VisualLogBaseModel", "VisualLogBaseModel is of abstract type.");
}

void ViewEngine::initializeBaseTypes(ViewEngine *engine){
    MetaInfo::Ptr ti = engine->registerQmlTypeInfo<lv::Group>(nullptr, nullptr, [](){return new Group;}, false);
    ti->addSerialization(&lv::Group::serialize, &lv::Group::deserialize);
}

QString ViewEngine::toErrorString(const QQmlError &error){
    QString result;
    result.sprintf(
        "\'%s\':%d,%d %s",
        qPrintable(error.url().toString()),
        error.line(),
        error.column(),
        qPrintable(error.description())
    );

    return result;
}

QString ViewEngine::toErrorString(const QList<QQmlError> &errors){
    QString result;
    foreach ( const QQmlError& error, errors ){
        result += toErrorString(error) + "\n";
    }

    return result;
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
        QObject *reference,
        QQmlContext* context,
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
        emit objectCreationError(toJSErrors(errors), url, reference, context);
        return;
    }

    if ( !context )
        context = m_engine->rootContext();

    QQmlIncubator incubator;
    component.create(incubator, context);
    setIsLoading(true);

    while (incubator.isLoading()) {
        QCoreApplication::processEvents(QEventLoop::AllEvents, 15);
    }

    QList<QQmlError> incubatorErrors = component.errors();
    if ( incubatorErrors.size() > 0 ){
        setIsLoading(false);
        QJSValue jsErrors = toJSErrors(incubatorErrors);
        m_engineMutex->unlock();
        emit objectCreationError(jsErrors, url, reference, context);
        return;
    }

    if ( incubator.isNull() || !incubator.object() ){
        setIsLoading(false);
        QQmlError errorObject;
        errorObject.setDescription("Component returned null object.");
        QJSValue jsErrors = toJSErrors(QList<QQmlError>() << errorObject);
        m_engineMutex->unlock();
        emit objectCreationError(jsErrors, url, reference, context);
        return;
    }

    QObject* obj = incubator.object();

    m_engine->setObjectOwnership(obj, QQmlEngine::JavaScriptOwnership);

    emit objectAcquired(url, reference);

    if (parent)
        obj->setParent(parent);

    QQuickItem *parentItem = qobject_cast<QQuickItem*>(parent);
    QQuickItem *item = qobject_cast<QQuickItem*>(obj);
    if (parentItem && item){
        item->setParentItem(parentItem);
    }

    for (auto it = m_compileHooks.begin(); it != m_compileHooks.end(); ++it)
        (it->m_hook)(qmlCode, url, obj, it->m_userData);

    setIsLoading(false);

    m_engineMutex->unlock();
    emit objectReady(obj, url, reference, context);
}

QJSValue ViewEngine::lastErrorsObject() const{
    return toJSErrors(lastErrors());
}

/**
 * \brief Creates an object from the given qmlcode synchronously
 */
QObject* ViewEngine::createObject(const QString &qmlCode, QObject *parent, const QUrl &url, bool clearCache){
    return createObject(qmlCode.toUtf8(), parent, url, clearCache);
}

/**
 * \brief Creates an object from the given qmlcode synchronously
 */
QObject *ViewEngine::createObject(const QByteArray &qmlCode, QObject *parent, const QUrl &file, bool clearCache){
    QMutexLocker engineMutexLock(m_engineMutex);

    if ( clearCache )
        m_engine->clearComponentCache();

    QQmlComponent component(m_engine);
    component.setData(qmlCode, file);

    m_lastErrors = component.errors();
    if ( !m_lastErrors.isEmpty() ){
        return nullptr;
    }

    QObject* obj = component.create(m_engine->rootContext());
    m_engine->setObjectOwnership(obj, QQmlEngine::JavaScriptOwnership);

    m_lastErrors = component.errors();
    if ( !m_lastErrors.isEmpty() ){
        return nullptr;
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

QObject *ViewEngine::createObject(const char *qmlCode, QObject *parent, const QUrl &file, bool clearCache){
    return createObject(QByteArray(qmlCode), parent, file, clearCache);
}

/** Throws errors on these warnings which don't have their own object  */
void ViewEngine::engineWarnings(const QList<QQmlError> &warnings){
    for ( auto it = warnings.begin(); it != warnings.end(); ++it ){
        const QQmlError& warning = *it;
        if ( warning.object() == nullptr ){
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

void ViewEngine::setPackageGraph(PackageGraph *pg){
    if ( m_packageGraph )
        THROW_EXCEPTION(lv::Exception, "Package graph has already been set.", Exception::toCode("PackageGraph"));
    m_packageGraph = pg;
}

QJSValue ViewEngine::toJSErrors(const QList<QQmlError> &errors) const{
    QJSValue val = m_engine->newArray(static_cast<uint>(errors.length()));
    uint i = 0;
    foreach( const QQmlError& error, errors ){
        val.setProperty(i++, toJSError(error));
    }
    return val;
}

}// namespace
