#include "qmlstreamvalueawait.h"
#include "qmlstreamfilter.h"

#include "live/qmlstreamprovider.h"
#include "live/exception.h"
#include "live/viewengine.h"

namespace lv{

QmlStreamValueAwait::QmlStreamValueAwait(QObject *parent)
    : QObject(parent)
    , m_isComponentComplete(false)
    , m_isRunning(false)
    , m_inStreamValue(false)
    , m_follow(nullptr)
    , m_stream(nullptr)
    , m_act(nullptr)
    , m_next(nullptr)
{
}

QmlStreamValueAwait::~QmlStreamValueAwait(){
    if ( m_stream ){
        Shared::unref(m_stream);
        m_stream->unsubscribeObject(this);
    }
}

void QmlStreamValueAwait::streamHandler(QObject *that, const QJSValue &val){
    QmlStreamValueAwait* sf = static_cast<QmlStreamValueAwait*>(that);
    sf->onStreamValue(val);
}

void QmlStreamValueAwait::classBegin(){
}

void QmlStreamValueAwait::componentComplete(){
    if ( m_stream )
        return;

    QmlStreamFilter* filter = qobject_cast<QmlStreamFilter*>(parent());
    if ( filter ){
        m_follow = filter;
        connect(filter, &QmlStreamFilter::pullChanged, this, &QmlStreamValueAwait::__updateFollowsObject);
        __updateFollowsObject();
    }
}

void QmlStreamValueAwait::setValueType(const QString &valueType){
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
    emit valueTypeChanged();
}

void QmlStreamValueAwait::__streamRemoved(){
    m_stream = nullptr;
}

void QmlStreamValueAwait::__updateFollowsObject(){
    QmlStreamFilter* filter = qobject_cast<QmlStreamFilter*>(m_follow);
    if ( filter ){
        if( m_stream )
            m_stream->unsubscribeObject(this);

        if ( filter->pull() ){
            m_stream = filter->pull();
            m_stream->forward(this, &QmlStreamValueAwait::streamHandler);
            emit streamChanged();
        }
    }
}

void QmlStreamValueAwait::__actResultReady(){
    m_isRunning = false;
    if ( m_stream && m_stream->provider() )
        m_stream->provider()->resume();
}

void QmlStreamValueAwait::onStreamValue(const QJSValue &val){
    if ( m_isRunning && m_act ){ // if there's an act set, wait for result
        return;
    }
    if ( m_inStreamValue){ // avoid recursive signal callbacks (binding loops) and return to event loop
        m_next = new QJSValue(val);
        if ( m_act && m_stream->provider() ){
            m_stream->provider()->wait();
        }
        return;
    }

    m_inStreamValue = true;
    m_isRunning = true;
    if ( m_act && m_stream->provider() ){
        m_stream->provider()->wait();
    }

    m_current = val;
    emit valueChanged();

    while ( m_next ) { // iterate as long as new values come along during recursive calls from valueChanged()
        m_current = *m_next;
        delete m_next;
        m_next = nullptr;
        emit valueChanged();
    }

    m_inStreamValue = false;
}

void QmlStreamValueAwait::removeFollowsObject(){
    QmlStreamFilter* filter = qobject_cast<QmlStreamFilter*>(m_follow);
    if ( filter ){
        disconnect(filter, &QmlStreamFilter::pullChanged, this, &QmlStreamValueAwait::__updateFollowsObject);
    }
    m_follow = nullptr;
}

void QmlStreamValueAwait::setStream(QmlStream *stream){
    if (m_stream == stream)
        return;

    if ( m_follow ){
        removeFollowsObject();
    }

    if( m_stream ){
        Shared::unref(m_stream);
        m_stream->unsubscribeObject(this);
        disconnect(m_stream, &QObject::destroyed, this, &QmlStreamValueAwait::__streamRemoved);
    }

    m_stream = stream;
    m_stream->forward(this, &QmlStreamValueAwait::streamHandler);
    connect(m_stream, &QObject::destroyed, this, &QmlStreamValueAwait::__streamRemoved);
    Shared::ref(m_stream);

    emit streamChanged();
}

void QmlStreamValueAwait::setAct(QmlAct *act){
    if (m_act == act)
        return;

    if ( m_act ){
        disconnect(m_act, &QmlAct::resultChanged, this, &QmlStreamValueAwait::__actResultReady);
    }

    m_act = act;
    if ( m_act ){
        connect(m_act, &QmlAct::resultChanged, this, &QmlStreamValueAwait::__actResultReady);
    }

    m_act = act;
    emit actChanged();


    m_act = act;
    emit actChanged();
}

}// namespace
