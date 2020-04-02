#include "qmlbindingspan.h"
#include "qmleditfragment.h"

#include <QObject>

namespace lv{

QmlBindingSpan::QmlBindingSpan(QmlEditFragment* fragment)
    : m_fragment(fragment)
{
}

QmlBindingSpan::~QmlBindingSpan(){
    if ( m_connectionChannel ){
        QObject::disconnect(m_connectionChannel.data(), &QmlBindingChannel::runnableObjectReady, nullptr, nullptr);
    }
}

void QmlBindingSpan::setConnectionChannel(QmlBindingChannel::Ptr bc){
    if ( m_connectionChannel ){
        QObject::disconnect(
            m_connectionChannel.data(), &QmlBindingChannel::runnableObjectReady,
            m_fragment, &QmlEditFragment::__inputRunnableObjectReady);
    }

    m_connectionChannel = bc;

    if ( m_connectionChannel ){
        QObject::connect(
            m_connectionChannel.data(), &QmlBindingChannel::runnableObjectReady,
            m_fragment, &QmlEditFragment::__inputRunnableObjectReady);
    }
}

void QmlBindingSpan::addChannel(QmlBindingChannel::Ptr bc){
    m_outputChannels.append(bc);
}

void QmlBindingSpan::commit(const QVariant &value){
    if ( m_connectionChannel && m_connectionChannel->canModify() ){
        if ( m_connectionChannel->listIndex() == -1 ){
            m_connectionChannel->property().write(value);
        }
    }
}

}// namespace
