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

#ifndef LVMETHOD_H
#define LVMETHOD_H

#include "live/elements/lvelementsglobal.h"
#include "live/elements/value.h"
#include "live/elements/function.h"

#include "live/meta/functionargs.h"
#include "live/meta/indextuple.h"

#include <functional>
#include <vector>
#include <string>
#include <tuple>

namespace lv{ namespace el{

template<typename C, typename RT, typename ...Args>
class Method : public Function{

public:
    typedef RT(C::*FunctionType)(Args...);
    using ReturnType = typename std::remove_const<typename std::remove_reference<RT>::type>::type;

    static_assert(
        std::is_same<ReturnType, bool>::value ||
        std::is_same<ReturnType, Value::Int32>::value ||
        std::is_same<ReturnType, Value::Int64>::value ||
        std::is_same<ReturnType, Value::Number>::value ||
        std::is_same<ReturnType, std::string>::value ||
        std::is_same<ReturnType, Callable>::value ||
        std::is_same<ReturnType, Object>::value ||
        std::is_same<ReturnType, ScopedValue>::value ||
        std::is_same<ReturnType, Value>::value ||
        std::is_pointer<ReturnType>::value,
        "Return type is not convertible to script value."
    );

public:
    Method(const std::function<RT(C*, Args...)>& f)
        : m_function(f)
    {
        m_less = nullptr;
        m_ptr = &Function::methodPtrImplementation<C>;
        m_totalArguments  = sizeof...(Args);
        m_declaration =
            TypeName<RT>::capture(TypeNameOptions::None) +
            "(" + TypeName<Args...>::capture(TypeNameOptions::None) + ")";
        m_unwrapFunction = [this](const Function::CallInfo& params){
            Method::callMethod(
                params.engine(),
                m_function,
                typename meta::make_indexes<Args...>::type(), params);
        };
    }

    RT callOn(C* that, Args... args){
        return m_function(that, args...);
    }

private:
    std::function<RT(C*, Args ...args)> m_function;
};

template<typename C, typename ...Args>
class Method<C, void, Args...> : public Function{

public:
    Method(const std::function<void(C*, Args...)>& f)
        : m_function(f)
    {
        m_less           = nullptr;
        m_ptr            = &Function::methodPtrImplementation<C>;
        m_totalArguments = sizeof...(Args);
        m_declaration = "void(" + TypeName<Args...>::capture(TypeNameOptions::None) + ")";
        m_unwrapFunction = [this](const Function::CallInfo& params){
            Method::callVoidMethod(
                m_function, typename meta::make_indexes<Args...>::type(), params
            );
            return ScopedValue(params.engine());
        };
    }


    void callOn(C* that, Args... args){
        m_function(that, args...);
    }

private:
    std::function<void(C*, Args ...args)> m_function;
};

template<typename C, typename RT>
class Method<C, RT, const Function::CallInfo&> : public Function{

public:
    typedef RT(*FunctionType)(const Function::CallInfo&);
    using ReturnType = typename std::remove_const<typename std::remove_reference<RT>::type>::type;

    static_assert(
        std::is_same<ReturnType, bool>::value ||
        std::is_same<ReturnType, Value::Int32>::value ||
        std::is_same<ReturnType, Value::Int64>::value ||
        std::is_same<ReturnType, Value::Number>::value ||
        std::is_same<ReturnType, std::string>::value ||
        std::is_same<ReturnType, Callable>::value ||
        std::is_same<ReturnType, Object>::value ||
        std::is_same<ReturnType, ScopedValue>::value ||
        std::is_same<ReturnType, Value>::value ||
        std::is_pointer<ReturnType>::value,
        "Return type is not convertible to script value."
    );

public:
    Method(const std::function<RT(C*, const Function::CallInfo&)>& f)
        : m_function(f)
    {
        m_less = nullptr;
        m_ptr = &Function::methodPtrImplementation<C>;
        m_totalArguments  = 0;
        m_declaration = TypeName<RT>::capture(TypeNameOptions::None) + "(Optional)";
        m_unwrapFunction = [this](const Function::CallInfo& params){
            Function::checkReturn(params.engine(), params, m_function(params.template that<C>(), params));
        };
    }

    RT callOn(C* that, const Function::CallInfo& ci){
        return m_function(that, ci);
    }

private:
    std::function<RT(C*, const Function::CallInfo&)> m_function;
};

template<typename C>
class Method<C, void, const Function::CallInfo&> : public Function{

public:
    Method(const std::function<void(C*, const Function::CallInfo&)>& f)
        : m_function(f)
    {
        m_less           = nullptr;
        m_ptr            = &Function::methodPtrImplementation<C>;
        m_totalArguments = 0;
        m_declaration = "void(Optional)";
        m_unwrapFunction = [this](const Function::CallInfo& params){
            m_function(params.template that<C>(), params);
            return ScopedValue(params.engine());
        };
    }


    void callOn(C* that, const Function::CallInfo& ci){ m_function(that, ci); }

private:
    std::function<void(C*, const Function::CallInfo&)> m_function;
};



}} // namespace lv, el

#endif // LVMETHOD_H
