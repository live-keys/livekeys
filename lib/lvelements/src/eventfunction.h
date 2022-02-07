/****************************************************************************
**
** Copyright (C) 2022 Dinu SV.
** This file is part of Livekeys Application.
**
** GNU Lesser General Public License Usage
** This file may be used under the terms of the GNU Lesser
** General Public License version 3 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPLv3 included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 3 requirements
** will be met: https://www.gnu.org/licenses/lgpl.html.
**
****************************************************************************/

#ifndef LVEVENTFUNCTION_H
#define LVEVENTFUNCTION_H

#include "live/elements/lvelementsglobal.h"
#include "live/elements/function.h"
#include "live/elements/method.h"
#include "live/elements/event.h"
#include "live/typename.h"

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
        m_declaration = "Event(" + TypeName<Args...>::capture(TypeNameOptions::None) + ")";
        m_unwrapFunction = [this](const Function::CallInfo& params){
            if ( params.length() != m_totalArguments ){
                params.throwError(params.engine(), "Invalid number of arguments supplied to event");
                return;
            }

            Function::callVoidMethod(m_function, typename meta::make_indexes<Args...>::type(), params);
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
