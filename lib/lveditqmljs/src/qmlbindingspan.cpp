#include "qmlbindingspan.h"
#include "qmleditfragment.h"

#include <QObject>

namespace lv{

QmlBindingSpan::QmlBindingSpan(QmlEditFragment* fragment)
    : m_fragment(fragment)
{
}

QmlBindingSpan::~QmlBindingSpan(){
    if ( m_inputChannel ){
        QObject::disconnect(m_inputChannel.data(), &QmlBindingChannel::runnableObjectReady, nullptr, nullptr);
    }
}

void QmlBindingSpan::setInputChannel(QmlBindingChannel::Ptr bc){
    if ( m_inputChannel ){
        QObject::disconnect(
            m_inputChannel.data(), &QmlBindingChannel::runnableObjectReady,
            m_fragment, &QmlEditFragment::__inputRunnableObjectReady);
    }

    m_inputChannel = bc;

    if ( m_inputChannel ){
        QObject::connect(
            m_inputChannel.data(), &QmlBindingChannel::runnableObjectReady,
            m_fragment, &QmlEditFragment::__inputRunnableObjectReady);
    }
}

void QmlBindingSpan::addOutputChannel(QmlBindingChannel::Ptr bc){
    m_outputChannels.append(bc);
}

void QmlBindingSpan::commit(const QVariant &value){
    for ( auto it = m_outputChannels.begin(); it != m_outputChannels.end(); ++it ){
        QmlBindingChannel::Ptr& bc = *it;
        if ( bc->canModify() ){
            if ( bc->listIndex() == -1 ){
                bc->property().write(value);
            }
        }
    }
}

}// namespace
