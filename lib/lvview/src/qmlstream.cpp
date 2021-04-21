#include "qmlstream.h"

#include "live/viewcontext.h"
#include "live/viewengine.h"
#include "live/visuallogqt.h"

namespace lv{

QmlStream::QmlStream(QObject *parent)
    : QObject(parent)
    , m_object(nullptr)
{
    m_engine = lv::ViewContext::instance().engine()->engine();
}

QmlStream::~QmlStream(){
}

void QmlStream::push(QObject *object){
    if ( m_callbackForward.isCallable() ){
        m_callbackForward.call(QJSValueList() << m_engine->newQObject(object));
    } else if ( m_objectForward.isWritable() ){
        m_objectForward.write(QVariant::fromValue(object));
    } else if ( m_object ){
        m_functionForward(m_object, m_engine->newQObject(object));
    }
}

void QmlStream::push(const QJSValue &value){
    if ( m_callbackForward.isCallable() ){
        m_callbackForward.call(QJSValueList() << value);
    } else if ( m_objectForward.isWritable() ){
        m_objectForward.write(value.toVariant());
    } else if ( m_object ){
        m_functionForward(m_object, value);
    }
}

void QmlStream::forward(QObject *object, std::function<void (QObject *, const QJSValue &)> fn){
    m_object = object;
    m_functionForward = fn;
}

void QmlStream::forward(const QJSValue &callback){
    if ( callback.isCallable() ){
        m_callbackForward = callback;
    } else if ( callback.isArray() && callback.property("length").toInt() == 2 ){
        QObject* ob = callback.property(0).toQObject();
        QString prop = callback.property(1).toString();

        m_objectForward = QQmlProperty(ob, prop);
    }
}

}// namespace
