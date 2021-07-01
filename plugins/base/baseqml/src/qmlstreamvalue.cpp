#include "qmlstreamvalue.h"
#include "qmlstreamfilter.h"
#include "live/exception.h"
#include "live/viewengine.h"

namespace lv{

QmlStreamValue::QmlStreamValue(QObject *parent)
    : QObject(parent)
    , m_isComponentComplete(false)
    , m_follow(nullptr)
    , m_stream(nullptr)
{
}

QmlStreamValue::~QmlStreamValue(){
    if ( m_stream )
        m_stream->unsubscribeObject(this);
}

void QmlStreamValue::streamHandler(QObject *that, const QJSValue &val){
    QmlStreamValue* sf = static_cast<QmlStreamValue*>(that);
    sf->m_current = val;
    emit sf->valueChanged();
}

void QmlStreamValue::classBegin(){
}

void QmlStreamValue::componentComplete(){
    if ( m_stream )
        return;

    QmlStreamFilter* filter = qobject_cast<QmlStreamFilter*>(parent());
    if ( filter ){
        m_follow = filter;
        connect(filter, &QmlStreamFilter::pullChanged, this, &QmlStreamValue::updateFollowsObject);
        updateFollowsObject();
    }
}

void QmlStreamValue::setValueType(const QString &valueType){
    if (m_valueType == valueType)
        return;

    if ( m_isComponentComplete ){
        Exception e = CREATE_EXCEPTION(lv::Exception, "StreamValue: Cannot set valueType after component is complete.", Exception::toCode("~StreamValue"));
        ViewEngine* engine = ViewEngine::grab(this);
        engine->throwError(&e, this);
        return;
    }

    if ( valueType == "qml/object" ){
        m_current = QJSValue(QJSValue::NullValue);
    }

    m_valueType = valueType;
    emit valueTypeChanged(m_valueType);
}

void QmlStreamValue::__streamRemoved(){
    m_stream = nullptr;
}

void QmlStreamValue::updateFollowsObject(){
    QmlStreamFilter* filter = qobject_cast<QmlStreamFilter*>(m_follow);
    if ( filter ){
        if( m_stream )
            m_stream->unsubscribeObject(this);

        if ( filter->pull() ){
            m_stream = filter->pull();
            m_stream->forward(this, &QmlStreamValue::streamHandler);
            emit streamChanged();
        }
    }
}

void QmlStreamValue::removeFollowsObject(){
    QmlStreamFilter* filter = qobject_cast<QmlStreamFilter*>(m_follow);
    if ( filter ){
        disconnect(filter, &QmlStreamFilter::pullChanged, this, &QmlStreamValue::updateFollowsObject);
    }
    m_follow = nullptr;
}

void QmlStreamValue::setStream(QmlStream *stream){
    if (m_stream == stream)
        return;

    if ( m_follow ){
        removeFollowsObject();
    }

    if( m_stream ){
        m_stream->unsubscribeObject(this);
        disconnect(m_stream, &QObject::destroyed, this, &QmlStreamValue::__streamRemoved);
    }

    m_stream = stream;
    m_stream->forward(this, &QmlStreamValue::streamHandler);
    connect(m_stream, &QObject::destroyed, this, &QmlStreamValue::__streamRemoved);

    emit streamChanged();
}

}// namespace
