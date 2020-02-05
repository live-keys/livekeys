#include "event.h"
#include "element.h"
#include "v8nowarnings.h"

namespace lv{ namespace el{

std::map<std::type_index, Event::Id> EventIdGenerator::registeredTypes;

Engine *EventListenerContainerBase::elementEngine(Element *element){
    return element->engine();
}

void InstanceEvent::callbackImplementation(const v8::FunctionCallbackInfo<v8::Value> &info){
    InstanceEvent* ie = reinterpret_cast<InstanceEvent*>(info.Data().As<v8::External>()->Value());

    Function::Parameters p(info.Length());
    for ( int i = 0; i < info.Length(); ++i ){
        p.assign(i, info[i]);
    }

    try {
        ie->call(p);
    }
    catch (lv::Exception exc)
    {
        ie->m_parent->engine()->throwError(&exc, ie->m_parent);
        return;
    }

}

void InstanceEvent::call(const Function::Parameters &params){
    if ( params.length() != m_numArguments )
    {
        THROW_EXCEPTION(lv::Exception, "Wrong number of function arguments", lv::Exception::toCode("~InstanceEvent"));
    }

    m_parent->notifyFromInstance(m_eventId, params);
}

void EventConnection::remove(EventConnection *conn){
    Element* emitter = conn->emitter();
    emitter->removeListeningConnection(conn);
}

}} // namespace lv, el
