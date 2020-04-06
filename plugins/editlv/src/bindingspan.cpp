#include "bindingspan.h"
#include "lveditfragment.h"

#include <QObject>

namespace lv{

BindingSpan::BindingSpan(LvEditFragment* fragment)
    : m_fragment(fragment)
{
}

BindingSpan::~BindingSpan(){
    if ( m_connectedChannel ){
        QObject::disconnect(m_connectedChannel.get(), &BindingChannel::runnableObjectReady, nullptr, nullptr);
    }
}

void BindingSpan::connectChannel(BindingChannel::Ptr bc){
    if ( m_connectedChannel ){
        QObject::disconnect(
            m_connectedChannel.get(), &BindingChannel::runnableObjectReady,
            m_fragment, &LvEditFragment::__inputRunnableObjectReady);
    }

    m_connectedChannel = bc;

    if ( m_connectedChannel ){
        QObject::connect(
            m_connectedChannel.get(), &BindingChannel::runnableObjectReady,
            m_fragment, &LvEditFragment::__inputRunnableObjectReady);
    }
}

void BindingSpan::commit(const QVariant &/*value*/){
    for ( auto it = m_channels.begin(); it != m_channels.end(); ++it ){
        BindingChannel::Ptr& bc = *it;
        if ( bc->canModify() ){
            if ( bc->listIndex() == -1 ){
                // bc->property()->write(value); // TODO: ELEMENTS #381
            }
        }
    }
}

}// namespace
