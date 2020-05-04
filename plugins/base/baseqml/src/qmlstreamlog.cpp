#include "qmlstreamlog.h"
#include "live/visuallogqt.h"

#include <QtDebug>

namespace lv{

QmlStreamLog::QmlStreamLog(QObject *parent)
    : QObject(parent)
    , m_stream(nullptr)
    , m_log(new VisualLogQmlObject(this))
{
}

void QmlStreamLog::setStream(QmlStream *stream){
    if (m_stream == stream)
        return;

    if ( m_stream ){
        m_stream->forward(nullptr, nullptr);
    }

    m_stream = stream;
    emit streamChanged();

    if ( m_stream ){
        m_stream->forward(this, &QmlStreamLog::onStreamData);
    }
}

void QmlStreamLog::onStreamData(QObject *that, const QJSValue &val){
    QmlStreamLog* t = static_cast<QmlStreamLog*>(that);
    t->m_log->i(val);
}

QmlStreamLog::~QmlStreamLog(){
}

}// namespace
