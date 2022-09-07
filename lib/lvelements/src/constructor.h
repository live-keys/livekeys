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

#ifndef LVCONSTRUCTOR_H
#define LVCONSTRUCTOR_H

#include "live/elements/lvelementsglobal.h"
#include "live/elements/function.h"

namespace lv{ namespace el{

template<typename C, typename ...Args> class ConcreteConstructor;

// Constructor
// -----------

/**
 * @brief Wraps an element constructor
 */
class LV_ELEMENTS_EXPORT Constructor{

public:
    typedef void(*FunctionPointer)(const v8::FunctionCallbackInfo<v8::Value>& value);
    typedef std::function<Element*(const Function::CallInfo&)> UnwrapFunction;

public:
    Constructor(FunctionPointer ptr, size_t argSize, Constructor* less)
        : m_ptr(ptr)
        , m_totalArguments(argSize)
        , m_less(less)
    {}

    template<class T> static void ptrImplementation(const v8::FunctionCallbackInfo<v8::Value>& info);
    static void nullImplementation(const v8::FunctionCallbackInfo<v8::Value>& info);

    static void bindLifeTimeWithObject(Element* e, const v8::FunctionCallbackInfo<v8::Value>& info);
    static void assignSelfReturnValue(const v8::FunctionCallbackInfo<v8::Value>& info);

    FunctionPointer ptr(){ return m_ptr; }

    Element* create(const Function::CallInfo& params){ return m_unwrapConstructor(params); }
    size_t totalArguments() const{ return m_totalArguments; }
    const std::string& getDeclaration() const{ return m_declaration; }
    Constructor* less() const { return m_less; }

    template<typename C, typename ...Args>
    static UnwrapFunction createUnwrapConstructor(ConcreteConstructor<C, Args...>* p);

protected:
    FunctionPointer m_ptr;

    std::function<Element*(const Function::CallInfo&)> m_unwrapConstructor;
    size_t       m_totalArguments;
    std::string  m_declaration;
    Constructor* m_less;
};

template<class T>
void Constructor::ptrImplementation(const v8::FunctionCallbackInfo<v8::Value>& info){
    Function::CallInfo pr(&info);
    Element* e = T::metaObject().constructor()->create(pr);
    Constructor::bindLifeTimeWithObject(e, info);
    Constructor::assignSelfReturnValue(info);
}

// ConcreteConstructor
// -------------------

template<typename C, typename ...Args>
class ConcreteConstructor : public Constructor{

public:
    friend class Constructor;

public:
    ConcreteConstructor()
        : Constructor(&Constructor::ptrImplementation<C>, sizeof...(Args), nullptr)
    {
        m_unwrapConstructor = Constructor::createUnwrapConstructor<C, Args...>(this);
        m_declaration = "(" + TypeName<Args...>::capture(TypeNameOptions::None) + ")";
    }

    static Element* createNative(Engine* engine, Args ...args){
        return new C(engine, args...);
    }
};

template<typename C, typename ...Args>
Constructor::UnwrapFunction Constructor::createUnwrapConstructor(ConcreteConstructor<C, Args...>* p){
    return [p](const Function::CallInfo& params){
        DISABLE_UNUSED_WARNING(p);
        //TODO: Check params size; throw error
        return ConcreteConstructor<C, Args...>::createNative(params.engine(), params.template getValue<0, Args>()...);
    };
}


}}// namespace lv, el

#endif // LVCONSTRUCTOR_H
