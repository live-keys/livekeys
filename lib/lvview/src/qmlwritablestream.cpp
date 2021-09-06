#include "qmlwritablestream.h"

namespace lv{

QmlWritableStream::QmlWritableStream(QObject *parent)
    : QObject(parent)
    , m_stream(new QmlStream())
{
    Shared::ref(m_stream);
}

QmlWritableStream::~QmlWritableStream(){
    Shared::unref(m_stream);
}

}// namespace
