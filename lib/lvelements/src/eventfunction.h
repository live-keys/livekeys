#ifndef LVEVENTFUNCTION_H
#define LVEVENTFUNCTION_H

#include "live/elements/lvelementsglobal.h"
#include "live/elements/function.h"
#include "live/elements/method.h"
#include "live/elements/event.h"

namespace lv{ namespace el{


class LV_ELEMENTS_EXPORT EventFunction : public Function{

public:
    EventFunction(Event::Id id);

    virtual void notify(const Function::CallInfo& params) = 0;
    virtual EventListenerContainerBase* generateListenerContainer() = 0;

    Event::Id eventId() const{ return m_eventId; }

private:
    Event::Id m_eventId;
};

template<typename C, typename ...Args>
class ConcreteEventFunction : public EventFunction{

public:
    typedef Event(C::*FunctionType)(Args...);

public:
    ConcreteEventFunction(Event::Id eventId, const std::function<Event(C*, Args...)>& f)
        : EventFunction(eventId)
        , m_function(f)
    {
        m_less = nullptr;
        m_ptr = &Function::methodPtrImplementation<C>;
        m_totalArguments  = sizeof...(Args);
        m_unwrapFunction = [this](const Function::CallInfo& params){
            if ( params.length() != m_totalArguments )
                throw std::exception();

            Function::callVoidMethod(m_function, typename meta::make_indexes<Args...>::type(), params);
            return LocalValue(params.engine());
        };
    }

    EventListenerContainerBase* generateListenerContainer(){
        return new EventListenerContainer<Args...>;
    }

    Event callOn(C* that, Args... args){
        return m_function(that, args...);
    }

    void notify(const CallInfo &params){
        m_unwrapFunction(params);
    }

private:
    std::function<Event(C*, Args ...args)> m_function;
};


}} // lv, script

#endif // LVEVENTFUNCTION_H
