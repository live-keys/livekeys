#include "bindingchannel.h"
#include "live/codepalette.h"
#include "live/runnable.h"
#include <QObject>

namespace lv{

BindingChannel::Ptr BindingChannel::create(el::BindingPath::Ptr bindingPath, Runnable *runnable){
    return BindingChannel::Ptr(new BindingChannel(bindingPath, runnable));
}

BindingChannel::Ptr BindingChannel::create(
        el::BindingPath::Ptr bindingPath, Runnable *runnable, el::Property *property, int listIndex)
{
    BindingChannel::Ptr bc = BindingChannel::create(bindingPath, runnable);
    bc->updateConnection(property, listIndex);
    return bc;
}

BindingChannel::BindingChannel(el::BindingPath::Ptr bindingPath, Runnable *runnable, QObject *parent)
    : QObject(parent)
    , m_bindingPath(bindingPath)
    , m_runnable(runnable)
    , m_listIndex(-1)
    , m_enabled(false)
{
    connect(runnable, &Runnable::objectReady, this, &BindingChannel::__runnableReady);
}

BindingChannel::~BindingChannel(){
}

BindingChannel::Ptr BindingChannel::traverseBindingPath()
{
    return nullptr;
}

void BindingChannel::__runnableReady(){

    auto bc = traverseBindingPath();
    if ( bc ){
        m_property = bc->m_property;
        m_listIndex = bc->m_listIndex;
    } else {
        m_property = nullptr;
        m_listIndex = -1;
    }

    emit runnableObjectReady();
}


}
