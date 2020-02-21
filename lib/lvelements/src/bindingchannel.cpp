#include "bindingchannel.h"
#include "live/codepalette.h"
#include "live/qmleditfragment.h"
#include "live/runnable.h"
#include "live/documentqmlinfo.h"

namespace lv{ namespace el {

BindingChannel::Ptr BindingChannel::create(BindingPath::Ptr bindingPath, Runnable *runnable){
    return BindingChannel::Ptr(new BindingChannel(bindingPath, runnable));
}

BindingChannel::Ptr BindingChannel::create(
        BindingPath::Ptr bindingPath, Runnable *runnable, Property *property, int listIndex)
{
    BindingChannel::Ptr bc = BindingChannel::create(bindingPath, runnable);
    bc->updateConnection(property, listIndex);
    return bc;
}

BindingChannel::BindingChannel(BindingPath::Ptr bindingPath, Runnable *runnable, Engine *parent)
    : Element(parent)
    , m_bindingPath(bindingPath)
    , m_runnable(runnable)
    , m_listIndex(-1)
    , m_enabled(false)
{
    // connect(runnable, &Runnable::objectReady, this, &BindingChannel::__runableReady); // TODO
}

BindingChannel::~BindingChannel(){
}

//TODO//void BindingChannel::__runableReady(){
//    BindingChannel::Ptr bc = DocumentQmlInfo::traverseBindingPath(m_bindingPath, m_runnable);
//    if ( bc ){
//        m_property = bc->m_property;
//        m_listIndex = bc->m_listIndex;
//    } else {
//        m_property = nullptr;
//        m_listIndex = -1;
//    }

//    emit runnableObjectReady();
//}


}
}
