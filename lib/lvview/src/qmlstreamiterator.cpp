#include "qmlstreamiterator.h"

namespace lv{

QmlStreamIterator::QmlStreamIterator(QObject *parent)
    : QObject(parent)
    , m_stream(nullptr)
{

}

void QmlStreamIterator::streamHandler(QObject *that, const QJSValue &val){
    QmlStreamIterator* sf = static_cast<QmlStreamIterator*>(that);
    sf->m_current = val;
    emit sf->currentChanged();
}

}// namespace
