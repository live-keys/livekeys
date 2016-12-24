#include "qlivecvengine.h"
#include "qlivecvincubationcontroller.h"

#include <QQmlComponent>
#include <QQmlIncubator>
#include <QQmlEngine>
#include <QQuickItem>
#include <QQmlIncubationController>
#include <QMutexLocker>

#include <QCoreApplication>

#include <QDebug>

namespace lcv{

QLiveCVEngine::QLiveCVEngine(QQmlEngine *engine, QObject *parent)
    : QObject(parent)
    , m_engine(engine)
    , m_incubator(new QQmlIncubator(QQmlIncubator::Asynchronous))
    , m_incubationController(new QLiveCVIncubationController)
{
    m_engine->setIncubationController(m_incubationController);
}

QLiveCVEngine::~QLiveCVEngine(){
}

void QLiveCVEngine::useEngine(std::function<void(QQmlEngine *)> call){
    QMutexLocker engineMutexLock(&m_engineMutex);
    call(m_engine);
}

const QList<QQmlError> &QLiveCVEngine::lastErrors() const{
    return m_lastErrors;
}

QJSValue QLiveCVEngine::lastErrorsObject() const{
    return toJSErrors(lastErrors());
}

void QLiveCVEngine::createObjectAsync(const QString &qmlCode, QObject *parent, const QUrl &url){
    QMutexLocker engineMutexLock(&m_engineMutex);

    emit aboutToCreateObject(url);

    QQmlComponent component(m_engine);
    component.setData(qmlCode.toUtf8(), url);

    QList<QQmlError> errors = component.errors();
    if ( errors.size() > 0 ){
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
        emit objectCreationError(toJSErrors(incubatorErrors));
        return;
    }

    if ( incubator.isNull() ){
        setIsLoading(false);
        QQmlError errorObject;
        errorObject.setDescription("Component returned null object.");
        emit objectCreationError(toJSErrors(QList<QQmlError>() << errorObject));
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

    setIsLoading(false);
    emit objectCreated(obj);
}

QObject* QLiveCVEngine::createObject(const QString &qmlCode, QObject *parent, const QUrl &url){
    QMutexLocker engineMutexLock(&m_engineMutex);

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

QJSValue QLiveCVEngine::toJSErrors(const QList<QQmlError> &errors) const{
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
