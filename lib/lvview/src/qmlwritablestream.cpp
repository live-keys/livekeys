#include "qmlwritablestream.h"

namespace lv{

QmlWritableStream::QmlWritableStream(QObject *parent)
    : QObject(parent)
    , m_stream(new QmlStream(this))
{
}

QmlWritableStream::~QmlWritableStream(){
}

}// namespace
