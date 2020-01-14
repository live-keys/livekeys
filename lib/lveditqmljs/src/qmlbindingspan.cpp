#include "qmlbindingspan.h"

namespace lv{

QmlBindingSpan::QmlBindingSpan(QmlEditFragment* fragment)
    : m_fragment(fragment)
{
}

QmlBindingSpan::~QmlBindingSpan(){
}

void QmlBindingSpan::setInputChannel(QmlBindingChannel::Ptr bc){
    //TODO: Check connections
    m_inputChannel = bc;
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
