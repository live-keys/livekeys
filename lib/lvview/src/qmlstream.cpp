#include "qmlstream.h"
#include <limits>

#include "live/viewcontext.h"
#include "live/viewengine.h"
#include "live/visuallogqt.h"

namespace lv{

// QmlStream::Observer
// ----------------------------------------------------------------------------

QmlStream::Observer::~Observer()
{
}

QmlStream::JsCallbackObserver::JsCallbackObserver(unsigned int id, QJSValue callback, QJSValue endCallback)
    : Observer(id, QmlStream::Observer::JsCallback)
    , m_callback(callback)
    , m_endCallback(endCallback)
{
}

void QmlStream::JsCallbackObserver::push(ViewEngine* engine, QObject *object){
    m_callback.call(QJSValueList() << engine->engine()->newQObject(object));
}

void QmlStream::JsCallbackObserver::close(){
    if ( m_endCallback.isCallable() )
        m_endCallback.call();
}

void QmlStream::JsCallbackObserver::push(ViewEngine*, const QJSValue &value){
    m_callback.call(QJSValueList() << value);
}



QmlStream::CppCallbackObserver::CppCallbackObserver(unsigned int id, QObject *object, QmlStream::CppCallbackObserver::CppCallback cb, CppEndCallback ecb)
    : Observer(id, QmlStream::Observer::CppCallback)
    , m_object(object)
    , m_callback(cb)
    , m_endCallback(ecb)
{
}

void QmlStream::CppCallbackObserver::push(ViewEngine *, const QJSValue &value){
    m_callback(m_object, value);
}

void QmlStream::CppCallbackObserver::push(ViewEngine *engine, QObject *object){
    m_callback(m_object, engine->engine()->newQObject(object));
}

void QmlStream::CppCallbackObserver::close(){
    if ( m_endCallback )
        m_endCallback(m_object);
}

QObject *QmlStream::CppCallbackObserver::object(){
    return m_object;
}

QmlStream::PropertyObserver::PropertyObserver(unsigned int id, QJSValue callback, QJSValue endCallback)
    : Observer(id, QmlStream::Observer::Property)
{
    QObject* ob = callback.property(0).toQObject();
    QString prop = callback.property(1).toString();

    m_callback = QQmlProperty(ob, prop);
    m_endCallback = endCallback;
}

void QmlStream::PropertyObserver::push(ViewEngine*, const QJSValue &value){
    m_callback.write(value.toVariant());
}

void QmlStream::PropertyObserver::push(ViewEngine*, QObject *object){
    m_callback.write(QVariant::fromValue(object));
}

void QmlStream::PropertyObserver::close(){
    if ( m_endCallback.isCallable() )
        m_endCallback.call();
}

// QmlStream
// ----------------------------------------------------------------------------

QmlStream::QmlStream(QObject *parent)
    : Shared(parent)
    , m_provider(nullptr)
    , m_idCounter(1)
    , m_closed(false)
    , m_observers(new std::list<QmlStream::Observer*>())
{
    //TODO: Require engine at constructor
    m_engine = lv::ViewContext::instance().engine();
}

QmlStream::QmlStream(QmlStreamProvider *provider, QObject *parent)
    : Shared(parent)
    , m_provider(provider)
    , m_idCounter(1)
    , m_closed(false)
    , m_observers(new std::list<QmlStream::Observer*>())
{
    //TODO: Require engine at constructor
    m_engine = lv::ViewContext::instance().engine();
}

QmlStream::~QmlStream(){
    delete m_observers;
}

void QmlStream::push(QObject *object){
    for(auto it = m_observers->begin(); it != m_observers->end(); ++it ){
        QmlStream::Observer* observer = *it;
        observer->push(m_engine, object);
    }
}

void QmlStream::push(const QJSValue &value){
    for(auto it = m_observers->begin(); it != m_observers->end(); ++it ){
        QmlStream::Observer* observer = *it;
        observer->push(m_engine, value);
    }
}

void QmlStream::close(){
    if ( m_closed )
        return;

    m_closed = true;
    for(auto it = m_observers->begin(); it != m_observers->end(); ++it ){
        QmlStream::Observer* observer = *it;
        observer->close();
    }
}

QmlStream::Observer* QmlStream::forward(QObject* object, QmlStream::CppCallbackObserver::CppCallback fn, QmlStream::CppCallbackObserver::CppEndCallback efn){
    QmlStream::Observer* observer = new QmlStream::CppCallbackObserver(m_idCounter, object, fn, efn);
    incrementIdCounter();
    m_observers->push_back(observer);
    return observer;
}

void QmlStream::unsubscribeObject(QObject *object){
    auto it = m_observers->begin();
    while ( it != m_observers->end() ){
        QmlStream::Observer* observer = *it;
        if ( observer->type() == QmlStream::Observer::CppCallback ){
            QmlStream::CppCallbackObserver* cppobserver = static_cast<QmlStream::CppCallbackObserver*>(observer);
            if ( cppobserver->object() == object ){
                delete observer;
                it = m_observers->erase(it);
                continue;
            }
        }
        ++it;
    }
}

void QmlStream::unsubscribe(QmlStream::Observer *observer){
    for(auto it = m_observers->begin(); it != m_observers->end(); ++it ){
        QmlStream::Observer* o = *it;
        if ( o == observer ){
            delete o;
            m_observers->erase(it);
            return;
        }
    }
}

QmlStreamProvider *QmlStream::provider(){
    return m_provider;
}

QmlStream *QmlStream::fromValue(const QJSValue &val){
    if ( val.isQObject() ){
        QObject* resultOb = val.toQObject();
        QmlStream* stream = qobject_cast<QmlStream*>(resultOb);
        return stream;
    }
    return nullptr;
}

QJSValue QmlStream::forward(const QJSValue &callback, const QJSValue &endCallback){
    if ( callback.isCallable() ){
        m_observers->push_back(new QmlStream::JsCallbackObserver(m_idCounter, callback, endCallback.isCallable() ? endCallback : QJSValue()));
    } else if ( callback.isArray() && callback.property("length").toInt() == 2 ){
        m_observers->push_back(new QmlStream::PropertyObserver(m_idCounter, callback, endCallback.isCallable() ? endCallback : QJSValue()));
    }
    QJSValue observerId(m_idCounter);
    incrementIdCounter();
    return observerId;
}

bool QmlStream::unsubscribe(const QJSValue &val){
    unsigned int id = val.toUInt();
    for(auto it = m_observers->begin(); it != m_observers->end(); ++it ){
        QmlStream::Observer* o = *it;
        if ( o->id() == id ){
            delete o;
            m_observers->erase(it);
            return true;
        }
    }
    return false;
}

void QmlStream::incrementIdCounter(){
    m_idCounter++;
    if ( m_idCounter == std::numeric_limits<unsigned int>::max() )
        m_idCounter = 1;
}

void QmlStream::clearObservers(){
    for(auto it = m_observers->begin(); it != m_observers->end(); ++it )
        delete *it;
    m_observers->clear();
}


}// namespace
