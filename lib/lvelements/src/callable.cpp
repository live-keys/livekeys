#include "callable.h"
#include "live/exception.h"
#include "live/elements/engine.h"
#include "live/elements/element.h"
#include "live/elements/component.h"
#include "element_p.h"
#include "context_p.h"
#include "v8.h"

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

    v8::Local<v8::Value> shared = data->Get(v8::String::NewFromUtf8(engine->isolate(), "__shared__"));

    if ( shared->IsExternal() ){
        m_d = reinterpret_cast<CallablePrivate*>(v8::Local<v8::External>::Cast(shared)->Value());
        m_ref = m_d->ref;
    } else {
        m_d = new CallablePrivate(engine, data);
        m_ref = new int;
        m_d->ref = m_ref;
        data->Set(v8::String::NewFromUtf8(engine->isolate(), "__shared__"), v8::External::New(engine->isolate(), m_d));
    }

    ++(*m_ref);
}

Callable::~Callable(){
    --(*m_ref);
    if ( *m_ref == 0 ){
        if ( !m_d->data.IsEmpty() ){
            v8::Local<v8::Function> v = m_d->data.Get(m_d->engine->isolate());
            v->Set(v8::String::NewFromUtf8(m_d->engine->isolate(), "__shared__"), v8::Undefined(m_d->engine->isolate()));
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

LocalValue Callable::call(Element *that, const Function::Parameters &params) const{
    v8::Local<v8::Function> local = m_d->data.Get(that->engine()->isolate());
    v8::Local<v8::Object> localThat = ElementPrivate::localObject(that);
    return LocalValue(local->Call(localThat, params.m_length, params.m_args));
}

LocalValue Callable::call(Engine *engine, const Function::Parameters &params) const{
    v8::Local<v8::Function> local = m_d->data.Get(engine->isolate());
    return LocalValue(local->Call(engine->currentContext()->asLocal()->Global(), params.m_length, params.m_args));
}

LocalValue Callable::callAsConstructor(Engine *engine, const Function::Parameters &params) const{
    v8::Local<v8::Function> local = m_d->data.Get(engine->isolate());
    return LocalValue(local->CallAsConstructor(engine->currentContext()->asLocal(), params.m_length, params.m_args).ToLocalChecked());
}

v8::Local<v8::Function> Callable::data() const{
    return m_d->data.Get(m_d->engine->isolate());
}




}} // namespace lv, el
