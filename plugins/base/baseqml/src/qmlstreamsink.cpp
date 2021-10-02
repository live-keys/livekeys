#include "qmlstreamsink.h"
#include "qmlstreamfilter.h"
#include "live/qmlstream.h"
#include "live/visuallogqt.h"
#include <QtDebug>

namespace lv{

QmlStreamSink::QmlStreamSink(QObject *parent)
    : QObject(parent)
    , m_target(nullptr)
{
}

QmlStreamSink::~QmlStreamSink(){
    if ( m_target ){
        Shared::unref(m_target);
    }
}

void QmlStreamSink::componentComplete(){
    QmlStreamFilter* sf = qobject_cast<QmlStreamFilter*>(parent());
    if ( sf ){
        m_target = sf->result()->stream();
        Shared::ref(m_target);
    }
}

void QmlStreamSink::setInput(const QJSValue &input){
    m_input = input;
    emit inputChanged();

    if ( m_target ){
        m_target->push(input);
    }
}

}// namespace
