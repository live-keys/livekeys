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

#include "callable.h"
#include "live/exception.h"
#include "live/elements/engine.h"
#include "live/elements/element.h"
#include "live/elements/component.h"
#include "element_p.h"
#include "context_p.h"
#include "v8nowarnings.h"

namespace lv{ namespace el{

// Callable implementation
// ----------------------------------------------------------------------

class CallablePrivate{

public:
    CallablePrivate(Engine* pengine, const v8::Local<v8::Function>& d)
        : engine(pengine)
    {
        data.Reset(engine->isolate(), d);
    }

public:
    int*                         ref;
    Engine*                      engine;
    v8::Persistent<v8::Function> data;
};

Callable::Callable(Engine *engine)
    : m_d(new CallablePrivate(engine, v8::Local<v8::Function>()))
    , m_ref(new int)
{
    m_d->ref = m_ref;
    ++(*m_ref);
}

Callable::Callable(const Callable &other)
    : m_d(other.m_d)
    , m_ref(other.m_ref)
{
    ++(*m_ref);
}


Callable::Callable(Engine* engine, const v8::Local<v8::Function> &data)
    : m_d(nullptr)
    , m_ref(nullptr)
{
    auto isolate = engine->isolate();
    auto context = isolate->GetCurrentContext();
    v8::Local<v8::Value> shared = data->Get(context, v8::String::NewFromUtf8(engine->isolate(), "__shared__").ToLocalChecked()).ToLocalChecked();

    if ( shared->IsExternal() ){
        m_d = reinterpret_cast<CallablePrivate*>(v8::Local<v8::External>::Cast(shared)->Value());
        m_ref = m_d->ref;
    } else {
        m_d = new CallablePrivate(engine, data);
        m_ref = new int;
        m_d->ref = m_ref;
        data->Set(
            context,
            v8::String::NewFromUtf8(engine->isolate(), "__shared__").ToLocalChecked(),
            v8::External::New(engine->isolate(), m_d)
        ).IsNothing();
    }

    ++(*m_ref);
}

Callable::~Callable(){

    auto isolate = m_d->engine->isolate();
    auto context = isolate->GetCurrentContext();
    --(*m_ref);
    if ( *m_ref == 0 ){
        if ( !m_d->data.IsEmpty() ){
            v8::Local<v8::Function> v = m_d->data.Get(m_d->engine->isolate());
            v->Set(
                context,
                v8::String::NewFromUtf8(m_d->engine->isolate(), "__shared__").ToLocalChecked(),
                v8::Undefined(m_d->engine->isolate())
            ).IsNothing();
            m_d->data.SetWeak();
        }

        delete m_ref;
        delete m_d;
    }
}

Callable &Callable::operator =(const Callable &other){
    if ( this != &other ){
        --(*m_ref);
        if ( *m_ref == 0 ){
            delete m_d;
            delete m_ref;
        }
        m_d = other.m_d;
        m_ref = other.m_ref;
        ++(*m_ref);
    }
    return *this;
}

bool Callable::operator ==(const Callable &other){
    return ( m_d == other.m_d && m_ref == other.m_ref );
}

bool Callable::isNull() const{
    if ( m_d->data.IsEmpty() )
        return true;
    return m_d->data.Get(m_d->engine->isolate())->IsNull();
}

bool Callable::isComponent() const{
    return m_d->engine->isElementConstructor(*this);
}

Component Callable::toComponent() const{
    if ( !isComponent() )
        THROW_EXCEPTION(lv::Exception, "Callable is not of component type.", 0);;
    return Component(m_d->engine, *this);
}

ScopedValue Callable::call(Element *that, const Function::Parameters &params) const{
    auto isolate = m_d->engine->isolate();
    auto context = isolate->GetCurrentContext();
    v8::Local<v8::Function> local = m_d->data.Get(that->engine()->isolate());
    v8::Local<v8::Object> localThat = ElementPrivate::localObject(that);
    return ScopedValue(local->Call(context, localThat, params.m_length, params.m_args).ToLocalChecked());
}

ScopedValue Callable::call(Engine *engine, const Function::Parameters &params) const{
    auto isolate = m_d->engine->isolate();
    auto context = isolate->GetCurrentContext();
    v8::Local<v8::Function> local = m_d->data.Get(engine->isolate());
    return ScopedValue(local->Call(context, engine->currentContext()->asLocal()->Global(), params.m_length, params.m_args).ToLocalChecked());
}

ScopedValue Callable::callAsConstructor(Engine *engine, const Function::Parameters &params) const{
    v8::Local<v8::Function> local = m_d->data.Get(engine->isolate());
    return ScopedValue(local->CallAsConstructor(engine->currentContext()->asLocal(), params.m_length, params.m_args).ToLocalChecked());
}

v8::Local<v8::Function> Callable::data() const{
    return m_d->data.Get(m_d->engine->isolate());
}




}} // namespace lv, el
