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

#include "live/viewengine.h"
#include "live/exception.h"
#include "live/errorhandler.h"
#include "live/incubationcontroller.h"
#include "live/packagegraph.h"
#include "live/applicationcontext.h"
#include "live/componentdeclaration.h"

#include "live/settings.h"
#include "live/memory.h"
#include "live/visuallogmodel.h"
#include "live/qmlvisuallog.h"
#include "live/qmlerror.h"

#include "layer.h"
#include "windowlayer.h"
#include "qmlstream.h"
#include "qmlwritablestream.h"
#include "qmlclipboard.h"
#include "qmlprojectinfo.h"
#include "qmlpromise.h"
#include "viewengineinterceptor_p.h"
#include "propertyparserhook_p.h"

#include "private/qqmlcontext_p.h"

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

namespace{

const char* linkErrorProgram =
    "(function(engine){"
        "return function(error, object){\n"
            "error.message = engine.markErrorObject(error, object);"
            "return error;"
        "}"
    "})";

const char* languageProgram =
    "(function(qmlLanguage){\n"
        "function Language(qmlLanguage, str){\n"
            "//qmlLanguage.lookupTranslation(str.constructor === Array ? str.join('') : str)\n"
            "this._str = str.constructor === Array ?  str.join('') : str\n"
         "}\n"
         "Language.prototype.format = function(){\n"
             "var args = arguments\n"
             "return this._str.replace(/%((%)|\\d+)/g, function (m) {\n"
                 "var key = m.substring(1)\n"
                 "if ( key === '%' ) return key\n"
                 "var index = parseInt(key)\n"
                 "return args[index]\n"
             "});\n"
         "}\n"
         "Language.translate = function(str){\n"
             "return new Language(qmlLanguage, str)\n"
         "}\n"
         "return Language.translate\n"
    "})\n";

const char* propertyReferenceProgram =
    "(function PropertyReference(path){\n"
        "this.path = path;\n"
    "})\n";

const char* prefProgram =
    "(function(){ "
        "var path = []; "
        "for ( var i = 0; i < arguments.length; ++i ) path.push(arguments[i]); "
        "return new PropertyReference(path); "
    "})\n";

}// namespace

class ViewEnginePrivate{
public:
    ViewEnginePrivate()
        : interceptor(nullptr)
        , networkInterceptor(nullptr)
        , urlInterceptor(nullptr)
    {}

    ViewEngineInterceptor *interceptor;
    QmlEngineInterceptor::Factory* networkInterceptor;
    QmlEngineInterceptor::UrlInterceptor* urlInterceptor;
};

/** Default constructor */
ViewEngine::ViewEngine(QQmlEngine *engine, FileIOInterface::Ptr fileIO, QObject *parent)
    : QObject(parent)
    , d_ptr(new ViewEnginePrivate)
    , m_engine(engine)
    , m_engineMutex(new QMutex)
    , m_incubator(new QQmlIncubator(QQmlIncubator::Asynchronous))
    , m_incubationController(new IncubationController)
    , m_packageGraph(nullptr)
    , m_fileIO(fileIO)
    , m_errorCounter(0)
    , m_logger(new QmlVisualLog(engine))
    , m_memory(new Memory(this))
{
    Q_D(ViewEngine);
    d->urlInterceptor = new QmlEngineInterceptor::UrlInterceptor(this);
    m_engine->setUrlInterceptor(d->urlInterceptor);
//    m_engine->addUrlInterceptor(d->urlInterceptor);

    m_engine->rootContext()->setContextProperty("engine", this);
    m_engine->globalObject().setProperty("vlog", m_engine->newQObject(m_logger));

    m_engine->setProperty("viewEngine", QVariant::fromValue(this));
    m_engine->setIncubationController(m_incubationController);
    m_engine->setOutputWarningsToStandardError(true);
    connect(m_engine, SIGNAL(warnings(QList<QQmlError>)), this, SLOT(engineWarnings(QList<QQmlError>)));
    m_errorType = m_engine->evaluate("Error");

    QJSValue markErrorConstructor = m_engine->evaluate(linkErrorProgram);
    QJSValue markErrorFn = markErrorConstructor.call(QJSValueList() << engine->newQObject(this));
    m_engine->globalObject().setProperty("linkError", markErrorFn);

    QJSValue languageConstructor = m_engine->evaluate(languageProgram);
    QJSValue languageFn = languageConstructor.call(QJSValueList() << QJSValue(QJSValue::NullValue));
    m_engine->globalObject().setProperty("lang", languageFn);

    QJSValue propertyReferenceClass = m_engine->evaluate(propertyReferenceProgram);
    m_engine->globalObject().setProperty("PropertyReference", propertyReferenceClass);

    QJSValue prefFn = m_engine->evaluate(prefProgram);
    m_engine->globalObject().setProperty("pref", prefFn);

//    connect(m_engine, &QQmlEngine::quit, QCoreApplication::instance(),
//                      &QCoreApplication::quit, Qt::QueuedConnection);

//#if (QT_VERSION > QT_VERSION_CHECK(5,8,0))
//    connect(m_engine, &QQmlEngine::exit, QCoreApplication::instance(),
//                      &QCoreApplication::exit, Qt::QueuedConnection);
//#endif
}

/** Default destructor */
ViewEngine::~ViewEngine(){
    Q_D(ViewEngine);
    m_engine->setUrlInterceptor(nullptr);
//    m_engine->removeUrlInterceptor(d->urlInterceptor);
    m_engine->setNetworkAccessManagerFactory(nullptr);
    m_engine->setProperty("viewEngine", QVariant());
    delete m_engineMutex;
    m_engine->deleteLater();
    delete m_memory;
    delete d->interceptor;
    delete d->urlInterceptor;
    delete d->networkInterceptor;
}

/** Displays the errors the engine had previously */
const QList<QQmlError> &ViewEngine::lastErrors() const{
    return m_lastErrors;
}

QByteArray ViewEngine::componentSource(const QString &componentPath) const{
    auto it = m_componentSources.find(componentPath);
    return it != m_componentSources.end() ? it.value() : QByteArray::fromStdString(m_fileIO->readFromFile(componentPath.toStdString()));
}

/** Evaluates the piece of code given the filename and line number */
QJSValue ViewEngine::evaluate(const QString &jsCode, const QString &fileName, int lineNumber){
    return m_engine->evaluate(jsCode, fileName, lineNumber);
}

void ViewEngine::throwError(const QmlError &error){
    storeError(error);
    m_engine->throwError(error.messageWithId());
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
    QmlError error(this, *e, object);
    throwError(error);
}

/**
 * \brief Variant of the same-named function that uses a QJSValue object along with the calling object
 */
void ViewEngine::throwError(const QJSValue &jsError, QObject *object){
    QmlError error(this, jsError);
    error.assignObject(object);
    throwError(error);
}

QString ViewEngine::markErrorObject(QJSValue error, QObject *object){
    QmlError e(this, error);
    e.assignObject(object);
    storeError(e);
    return e.messageWithId();
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

void ViewEngine::setInterceptor(ViewEngineInterceptor *interceptor){
    Q_D(ViewEngine);
    if ( d->interceptor == interceptor )
        return;

    delete d->networkInterceptor;
    delete d->interceptor;

    d->interceptor = interceptor;
    d->urlInterceptor->setInterceptor(interceptor);
    d->networkInterceptor = new QmlEngineInterceptor::Factory(interceptor);
    m_engine->setNetworkAccessManagerFactory(d->networkInterceptor);
}

ComponentDeclaration ViewEngine::rootDeclaration(QObject *object) const{
    QQmlContext* ctx = qmlContext(object);

    QQmlContextData* data = QQmlContextPrivate::get(ctx)->data;
//    QQmlRefPointer<QQmlContextData> data = QQmlContextData::get(ctx);

    QUrl url = data->url();
    QString objectId = data->findObjectId(object);

//    QQmlRefPointer<QQmlContextData> child = data->childContexts();
    QQmlContextData* child = data->childContexts;

    while (child) {
        if ( !child->url().isEmpty() && child->contextObject == object ){
            url = child->url();
            objectId = child->findObjectId(object);
        }
        child = child->nextChild;
    }

    return ComponentDeclaration(objectId, url);
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
    qmlRegisterType<lv::QmlVariantList>(        uri, 1, 0, "VariantList");
    qmlRegisterType<lv::QmlObjectList>(         uri, 1, 0, "ObjectList");
    qmlRegisterType<lv::QmlVariantListModel>(   uri, 1, 0, "VariantListModel");
    qmlRegisterType<lv::QmlObjectListModel>(    uri, 1, 0, "ObjectListModel");
    qmlRegisterType<lv::WindowLayer>(           uri, 1, 0, "WindowLayer");
    qmlRegisterType<lv::QmlClipboard>(          uri, 1, 0, "Clipboard");
    qmlRegisterType<lv::QmlStream>(             uri, 1, 0, "Stream");
    qmlRegisterType<lv::QmlWritableStream>(     uri, 1, 0, "WritableStream");

    qmlRegisterUncreatableType<lv::Shared>(
        uri, 1, 0, "Shared", "Shared is of abstract type.");
    qmlRegisterUncreatableType<lv::Layer>(
        uri, 1, 0, "Layer", "Layer is of abstract type.");
    qmlRegisterUncreatableType<lv::QmlPromise>(
        uri, 1, 0, "Promise", "Promise can be created via base.PromiseOp.create().");

    qmlRegisterUncreatableType<lv::ViewEngine>(
        uri, 1, 0, "LiveEngine",      ViewEngine::typeAsPropertyMessage("LiveEngine", "engine"));
    qmlRegisterUncreatableType<lv::QmlProjectInfo>(
        uri, 1, 0, "ProjectInfo",     ViewEngine::typeAsPropertyMessage("ProjectInfo", "project"));
    qmlRegisterUncreatableType<lv::Settings>(
        uri, 1, 0, "LiveSettings",    ViewEngine::typeAsPropertyMessage("LiveSettings", "lk.settings"));
    qmlRegisterUncreatableType<lv::VisualLogModel>(
        uri, 1, 0, "VisualLogModel",  ViewEngine::typeAsPropertyMessage("VisualLogModel", "lk.log"));
    qmlRegisterUncreatableType<lv::Memory>(
        uri, 1, 0, "Memory",          ViewEngine::typeAsPropertyMessage("Memory", "engine.mem"));
    qmlRegisterUncreatableType<lv::QmlVisualLog>(
        uri, 1, 0, "VisualLog",       ViewEngine::typeAsPropertyMessage("VisualLog", "vlog"));
    qmlRegisterUncreatableType<lv::VisualLogBaseModel>(
        uri, 1, 0, "VisualLogBaseModel", "VisualLogBaseModel is of abstract type.");
}

QJSValue ViewEngine::typeDefaultValue(const QString &ti, lv::ViewEngine *engine){
    if ( ti.contains('#') )
        return QJSValue(QJSValue::NullValue);
    else if ( ti == "qml/object" )
        return QJSValue(QJSValue::NullValue);
    else if ( ti == "qml/bool" )
        return false;
    else if ( ti == "qml/double" || ti == "qml/int" || ti == "qml/enumeration" || ti == "qml/real" )
        return 0;
    else if ( ti == "qml/string" || ti == "qml/url")
        return QJSValue("");
    else if ( ti == "qml/var" )
        return QJSValue(QJSValue::UndefinedValue);

    if (!engine)
        return QJSValue(QJSValue::NullValue);

    if ( ti == "qml/color" ){
        QJSValue createColor = engine->engine()->globalObject().property("Qt").property("rgba");
        return createColor.call(QJSValueList() << 0 << 0 << 0 << 0);
    } else if ( ti == "qml/point" ){
        QJSValue createPoint = engine->engine()->globalObject().property("Qt").property("point");
        return createPoint.call(QJSValueList() << 0 << 0);
    } else if ( ti == "qml/size" ){
        QJSValue createSize = engine->engine()->globalObject().property("Qt").property("size");
        return createSize.call(QJSValueList() << 0 << 0);
    } else if ( ti == "qml/rect" ){
        QJSValue createRect = engine->engine()->globalObject().property("Qt").property("rect");
        return createRect.call(QJSValueList() << 0 << 0 << 0 << 0);
    }
    return QJSValue(QJSValue::UndefinedValue);;
}

QString ViewEngine::toErrorString(const QQmlError &error){
    QString result = QString::asprintf(
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

    setIsLoading(false);

    m_engineMutex->unlock();
    emit objectReady(obj, url, reference, context);
}

QJSValue ViewEngine::lastErrorsObject() const{
    return toJSErrors(lastErrors());
}

void ViewEngine::storeError(const QmlError &error){
    if ( m_errorCounter > 9999 )
        m_errorCounter = 0;
    int idNumber = m_errorCounter++;
    error.m_id = QString("%1").arg(idNumber, 5, 10, QChar('0'));
    m_errorObjects[error.m_id] = error;
}

bool ViewEngine::propagateError(const QmlError &error){
    QObject* errorHandler = error.object();
    while ( errorHandler != nullptr ){
        auto it = m_errorHandlers.find(errorHandler);
        if ( it != m_errorHandlers.end() ){
            it.value()->signalError(error.value());
            return true;
        }
        errorHandler = errorHandler->parent();
    }
    return false;
}

QQmlCustomParser *ViewEngine::createParserHook(
        const ViewEngine::ParsedPropertyValidator &propertyValidator,
        const ViewEngine::ParsedPropertyHandler &propertyHandler,
        const ViewEngine::ParsedChildrenHandler &childrenHandler,
        const ViewEngine::ParserReadyHandler &readyHandler)
{
    return new PropertyParserHook(propertyValidator, propertyHandler, childrenHandler, readyHandler);
}

/**
 * \brief Creates an object from the given qmlcode synchronously
 */
QObject* ViewEngine::createObject(const QString &qmlCode, QObject *parent, const QUrl &url, bool clearCache){
    return createObject(qmlCode.toUtf8(), parent, url, clearCache);
}

QQmlComponent *ViewEngine::createComponent(const QString &qmlCode, const QUrl &file){
    QQmlComponent* component = new QQmlComponent(m_engine);
    component->setData(qmlCode.toUtf8(), file);
    return component;
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
            QmlError error = findError(description);
            if ( !error.isNull() ){
                bool propagation = propagateError(error);
                if ( !propagation ){
                    emit applicationError(error.value());
                    continue;
                }
            }
        }
        emit applicationError(QmlError(this, *it).value());
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

void ViewEngine::printTrace(QJSEngine *engine){
    engine->globalObject().property("console").property("trace").call();
}

QmlError ViewEngine::findError(const QString &message) const{
    int startPos = message.lastIndexOf("(id:");
    int endPos  =  message.lastIndexOf(")");
    if ( startPos >= 0 && endPos > 0 ){
        QString errorId = message.mid(startPos + 4, endPos - startPos - 4);
        auto it = m_errorObjects.find(errorId);
        if ( it != m_errorObjects.end() ){
            return it.value();
        }
    }

    return QmlError();
}

QmlError ViewEngine::findError(QJSValue error) const{
    return findError(error.property("message").toString());
}

ViewEngine *ViewEngine::grab(const QObject *object){
    QQmlEngine* engine = qmlEngine(object);
    return grabFromQmlEngine(engine);
}

ViewEngine *ViewEngine::grabFromQmlEngine(QQmlEngine *engine){
    if (engine)
        return qobject_cast<ViewEngine*>(engine->property("viewEngine").value<QObject*>());
    return nullptr;
}

ViewEngine::ComponentResult::Ptr ViewEngine::createPluginObject(const QString &filePath, QObject *parent){
    return createObject(QString::fromStdString(lv::ApplicationContext::instance().pluginPath()) + "/" + filePath, parent);
}

ViewEngine::ComponentResult::Ptr ViewEngine::createObject(const QString &filePath, QObject *parent, QQmlContext* context){
    QFile f(filePath);
    if ( !f.open(QFile::ReadOnly) ){
        ViewEngine::ComponentResult::Ptr result = ViewEngine::ComponentResult::create();
        result->errors.append(QmlError(
            this,
            CREATE_EXCEPTION(
                lv::Exception,
                "Component: Failed to read file:" + f.fileName().toStdString(),
                Exception::toCode("~File"))
        ));
        return result;
    }

    QByteArray contentBytes = f.readAll();

    return createObject(f.fileName(), contentBytes, parent, context);
}

ViewEngine::ComponentResult::Ptr ViewEngine::createObject(const QUrl &filePath, QObject *parent, QQmlContext* context){
    if ( filePath.isLocalFile() ){
        return createObject(filePath.toLocalFile(), parent, context);
    }

    return ViewEngine::ComponentResult::create();
}

ViewEngine::ComponentResult::Ptr ViewEngine::createObject(const QString &filePath, const QByteArray &source, QObject *parent, QQmlContext *context){
    ViewEngine::ComponentResult::Ptr result = ViewEngine::ComponentResult::create();

    result->component = new QQmlComponent(m_engine, parent);
    result->component->setData(source, QUrl::fromLocalFile(filePath));

    m_componentSources[filePath] = source;

    QList<QQmlError> errors = result->component->errors();
    if ( errors.size() ){
        for ( const QQmlError& e : errors ){
            result->errors.append(QmlError(this, e));
        }
        return result;
    }

    if ( !result->component->isReady() ){
        m_componentSources.remove(filePath);
        result->errors.append(QmlError(
            this,
            CREATE_EXCEPTION(
                lv::Exception,
                "Component: Component is not ready:" + filePath.toStdString(),
                Exception::toCode("~Component"))
        ));
        return result;
    }


    result->object = result->component->create(context);
    m_componentSources.remove(filePath);

    errors = result->component->errors();
    if ( errors.size() ){
        for ( const QQmlError& e : errors ){
            result->errors.append(QmlError(this, e));
        }
    }
    return result;
}

ViewEngine::ComponentResult::Ptr ViewEngine::compileJsModule(const QByteArray &imports, const QByteArray &source, const QString &moduleFile){
    QByteArray qtqmlImport = imports.contains("import QtQml 2.3") ? "" : "import QtQml 2.3\n";

    QByteArray objectSource =
        qtqmlImport + imports + "\n" +
        "QtObject{\n" +
        "  property var exports: " + source +
        "\n}\n";

    return createObject(moduleFile, objectSource, nullptr);
}

QJSValue ViewEngine::unwrapError(QJSValue error) const{
    QmlError e = findError(error);
    return e.isNull() ? error : e.value();
}

QJSValue ViewEngine::toJSErrors(const QList<QQmlError> &errors) const{
    QJSValue val = m_engine->newArray(static_cast<uint>(errors.length()));
    uint i = 0;
    foreach( const QQmlError& error, errors ){
        val.setProperty(i++, toJSError(error));
    }
    return val;
}

ViewEngine::ComponentResult::Ptr ViewEngine::ComponentResult::create(QObject *o, QQmlComponent *c){
    return ViewEngine::ComponentResult::Ptr(new ViewEngine::ComponentResult(o, c));
}

ViewEngine::ComponentResult::~ComponentResult(){
    delete component;
}

void ViewEngine::ComponentResult::jsThrowError(){
    if ( errors.size() ){
        QmlError error = QmlError::join(errors);
        error.jsThrow();
    }
}

}// namespace
