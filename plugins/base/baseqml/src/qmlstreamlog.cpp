#include "qmlstreamlog.h"
#include "live/visuallogqt.h"

#include <QtDebug>

namespace lv{

QmlStreamLog::QmlStreamLog(QObject *parent)
    : QObject(parent)
    , m_stream(nullptr)
{
}

void QmlStreamLog::setStream(QmlStream *stream){
    if (m_stream == stream)
        return;

    if ( m_stream ){
        m_stream->unsubscribeObject(this);
        Shared::unref(m_stream);
    }

    m_stream = stream;
    emit streamChanged();

    if ( m_stream ){
        Shared::ref(m_stream);
        m_stream->forward(this, &QmlStreamLog::onStreamData);
    }
}

void QmlStreamLog::onStreamData(QObject *that, const QJSValue &val){
    QmlStreamLog* t = static_cast<QmlStreamLog*>(that);
    VisualLog vl(VisualLog::MessageInfo::Info);
    if ( !t->prefix().isEmpty() )
        vl << t->prefix();
    VisualLogQmlObject::logValue(vl, val);
}

QmlStreamLog::~QmlStreamLog(){
    if ( m_stream ){
        m_stream->unsubscribeObject(this);
        Shared::unref(m_stream);
    }
}

}// namespace
