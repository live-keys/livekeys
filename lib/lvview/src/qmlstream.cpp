#include "qmlstream.h"

#include "live/viewcontext.h"
#include "live/viewengine.h"

#include <QDebug>

namespace lv{

QmlStream::QmlStream(QObject *parent)
    : QObject(parent)
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
    }
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
