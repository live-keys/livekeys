#include "bindingspan.h"
#include "lveditfragment.h"

#include <QObject>

namespace lv{

BindingSpan::BindingSpan(LvEditFragment* fragment)
    : m_fragment(fragment)
{
}

BindingSpan::~BindingSpan(){
    if ( m_inputChannel ){
        QObject::disconnect(m_inputChannel.get(), &BindingChannel::runnableObjectReady, nullptr, nullptr);
    }
}

void BindingSpan::setInputChannel(BindingChannel::Ptr bc){
    if ( m_inputChannel ){
        QObject::disconnect(
            m_inputChannel.get(), &BindingChannel::runnableObjectReady,
            m_fragment, &LvEditFragment::__inputRunnableObjectReady);
    }

    m_inputChannel = bc;

    if ( m_inputChannel ){
        QObject::connect(
            m_inputChannel.get(), &BindingChannel::runnableObjectReady,
            m_fragment, &LvEditFragment::__inputRunnableObjectReady);
    }
}

void BindingSpan::addOutputChannel(BindingChannel::Ptr bc){
    m_outputChannels.append(bc);
}

void BindingSpan::commit(const QVariant &value){
    for ( auto it = m_outputChannels.begin(); it != m_outputChannels.end(); ++it ){
        BindingChannel::Ptr& bc = *it;
        if ( bc->canModify() ){
            if ( bc->listIndex() == -1 ){
                // bc->property()->write(value); // TODO: ELEMENTS #381
            }
        }
    }
}

}// namespace
