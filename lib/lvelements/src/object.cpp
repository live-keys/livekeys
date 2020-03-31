#include "object.h"
#include "live/elements/engine.h"
#include "live/elements/element.h"
#include "element_p.h"
#include "context_p.h"
#include "value_p.h"
#include "v8nowarnings.h"

namespace lv{ namespace el{


// Object implementation
// ----------------------------------------------------------------------

class ObjectPrivate{

public:
    ObjectPrivate(Engine* pengine, const v8::Local<v8::Object>& val)
        : engine(pengine)
    {
        data.Reset(engine->isolate(), val);
    }

public:
    Engine*                    engine;
    v8::Persistent<v8::Object> data;
    int*                       ref;
};


Object::Object(Engine* engine)
    : m_d(new ObjectPrivate(engine, v8::Local<v8::Object>()))
    , m_ref(new int)
{
    m_d->ref = m_ref;
    ++(*m_ref);
}

Object::Object(Engine* engine, const v8::Local<v8::Object> &data)
    : m_d(nullptr)
    , m_ref(nullptr)
{
    v8::Local<v8::Value> shared = data->Get(v8::String::NewFromUtf8(engine->isolate(), "__shared__"));

    if ( shared->IsExternal() ){
        m_d = reinterpret_cast<ObjectPrivate*>(v8::Local<v8::External>::Cast(shared)->Value());
        m_ref = m_d->ref;
    } else {
        m_d = new ObjectPrivate(engine, data);
        m_ref = new int;
        m_d->ref = m_ref;
        v8::Maybe<bool> result = data->DefineOwnProperty(
            engine->currentContext()->asLocal(),
            v8::String::NewFromUtf8(engine->isolate(), "__shared__"),
            v8::External::New(engine->isolate(), m_d),
            v8::DontEnum
        );
        if ( result.IsNothing() || !result.ToChecked() ){
            THROW_EXCEPTION(
                Exception,
                "Object::Object: Assertion failed when defining the '__shared__' property. This"
                "will result in undefined behaviour.",
                Exception::toCode("~Assertion")
            );
        }
    }

    ++(*m_ref);
}

Object::~Object(){
    --(*m_ref);
    if ( *m_ref == 0 ){
        if ( !m_d->data.IsEmpty() ){
            v8::Local<v8::Object> v = m_d->data.Get(m_d->engine->isolate());
            v->Set(v8::String::NewFromUtf8(m_d->engine->isolate(), "__shared__"), v8::Undefined(m_d->engine->isolate()));
            m_d->data.SetWeak();
        }

        delete m_ref;
        delete m_d;
    }
}

Object Object::create(Engine *engine){
    return Object(engine, v8::Object::New(engine->isolate()));
}

Object Object::createArray(Engine *engine, int length){
    return Object(engine, v8::Array::New(engine->isolate(), length));
}

std::string Object::toString() const{
    if ( m_d->data.IsEmpty() )
        return std::string();

    v8::Local<v8::Object> s = m_d->data.Get(m_d->engine->isolate());
    return *v8::String::Utf8Value(s->ToString(m_d->engine->isolate()));
}

Buffer Object::toBuffer() const{
    if ( m_d->data.IsEmpty() )
        return Buffer(nullptr, 0);

    v8::Local<v8::Object> b = m_d->data.Get(m_d->engine->isolate());
    return Buffer(v8::Local<v8::ArrayBuffer>::Cast(b));
}

Object Object::create(ObjectPrivate *d, int *ref){
    return Object(d, ref);
}

Object::Object(ObjectPrivate *d, int *ref)
    : m_d(d)
    , m_ref(ref){
    ++(*m_ref);
}

Object::Object(const Object &other)
    : m_d(other.m_d)
    , m_ref(other.m_ref)
{
    ++(*m_ref);
}

Object &Object::operator =(const Object &other){
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

bool Object::operator ==(const Object &other){
    return ( m_d == other.m_d && m_ref == other.m_ref );
}

bool Object::isNull() const{
    if ( m_d->data.IsEmpty() )
        return true;
    return m_d->data.Get(m_d->engine->isolate())->IsNullOrUndefined();
}

bool Object::isString() const{
    v8::Local<v8::Object> lo = m_d->data.Get(m_d->engine->isolate());
    return lo->IsString() || lo->IsStringObject();
}

bool Object::isPoint() const{
    v8::Local<v8::Object> lo = m_d->data.Get(m_d->engine->isolate());
    v8::Local<v8::FunctionTemplate> lotpl = m_d->engine->pointTemplate();
    return lotpl->HasInstance(lo);
}

void Object::toPoint(double &x, double &y) const{
    v8::Local<v8::Object> lo = m_d->data.Get(m_d->engine->isolate());
    if ( !isPoint() ){
        lv::Exception e = CREATE_EXCEPTION(lv::Exception, "Object is not of Point type.", 1);
        m_d->engine->throwError(&e, nullptr);
        return;
    }

    x = lo->Get(v8::String::NewFromUtf8(m_d->engine->isolate(), "x", v8::String::kInternalizedString))->NumberValue();
    y = lo->Get(v8::String::NewFromUtf8(m_d->engine->isolate(), "y", v8::String::kInternalizedString))->NumberValue();
}

bool Object::isSize() const{
    v8::Local<v8::Object> lo = m_d->data.Get(m_d->engine->isolate());
    v8::Local<v8::FunctionTemplate> lotpl = m_d->engine->sizeTemplate();
    return lotpl->HasInstance(lo);
}

void Object::toSize(double &width, double &height) const{
    v8::Local<v8::Object> lo = m_d->data.Get(m_d->engine->isolate());
    if ( !isSize() ){
        lv::Exception e = CREATE_EXCEPTION(lv::Exception, "Object is not of Size type.", 1);
        m_d->engine->throwError(&e, nullptr);
        return;
    }

    width =  lo->Get(v8::String::NewFromUtf8(m_d->engine->isolate(), "width", v8::String::kInternalizedString))->NumberValue();
    height = lo->Get(v8::String::NewFromUtf8(m_d->engine->isolate(), "height", v8::String::kInternalizedString))->NumberValue();
}

bool Object::isRectangle() const{
    v8::Local<v8::Object> lo = m_d->data.Get(m_d->engine->isolate());
    v8::Local<v8::FunctionTemplate> lotpl = m_d->engine->rectangleTemplate();
    return lotpl->HasInstance(lo);
}

void Object::toRectangle(double &x, double &y, double &width, double &height){
    v8::Local<v8::Object> lo = m_d->data.Get(m_d->engine->isolate());
    if ( !isRectangle() ){
        lv::Exception e = CREATE_EXCEPTION(lv::Exception, "Object is not of Rectangle type.", 1);
        m_d->engine->throwError(&e, nullptr);
        return;
    }

    x = lo->Get(v8::String::NewFromUtf8(m_d->engine->isolate(), "x", v8::String::kInternalizedString))->NumberValue();
    y = lo->Get(v8::String::NewFromUtf8(m_d->engine->isolate(), "y", v8::String::kInternalizedString))->NumberValue();
    width = lo->Get(v8::String::NewFromUtf8(m_d->engine->isolate(), "width", v8::String::kInternalizedString))->NumberValue();
    height = lo->Get(v8::String::NewFromUtf8(m_d->engine->isolate(), "height", v8::String::kInternalizedString))->NumberValue();
}

bool Object::isDate() const{
    v8::Local<v8::Object> lo = m_d->data.Get(m_d->engine->isolate());
    return lo->IsDate();
}

double Object::toDate() const{
    v8::Local<v8::Object> lo = m_d->data.Get(m_d->engine->isolate());
    return v8::Local<v8::Date>::Cast(lo)->ValueOf();
}

v8::Local<v8::Object> Object::data() const{
    return m_d->data.Get(m_d->engine->isolate());
}



// Object::Accessor implementation
// ----------------------------------------------------------------------

class Object::AccessorPrivate{
public:
    AccessorPrivate(const v8::Local<v8::Object>& o) : data(o){}

    v8::Local<v8::Object> data;
};


Object::Accessor::Accessor(Object o)
    : m_d(new Object::AccessorPrivate(o.data()))
{
}

Object::Accessor::Accessor(Context *context)
    : m_d(new Object::AccessorPrivate(context->asLocal()->Global()))
{
}

Object::Accessor::Accessor(const v8::Local<v8::Object> &vo)
    : m_d(new Object::AccessorPrivate(vo))
{
}

Object::Accessor::~Accessor(){
    delete m_d;
}

ScopedValue Object::Accessor::get(int index){
    return ScopedValue(m_d->data->Get(static_cast<uint32_t>(index)));
}

ScopedValue Object::Accessor::get(const ScopedValue &key){
    return m_d->data->Get(key.data());
}

ScopedValue Object::Accessor::get(Engine *engine, const std::string &str){
    return get(ScopedValue(engine, str));
}

void Object::Accessor::set(int index, const ScopedValue &value){
    m_d->data->Set(static_cast<uint32_t>(index), value.data());
}

void Object::Accessor::set(const ScopedValue &key, const ScopedValue &value){
    m_d->data->Set(key.data(), value.data());
}

void Object::Accessor::set(Engine *engine, const std::string &key, const ScopedValue &value){
    set(ScopedValue(engine, key), value);
}

bool Object::Accessor::has(Engine* engine, const ScopedValue &key) const{
    v8::Maybe<bool> result = m_d->data->HasOwnProperty(engine->currentContext()->asLocal(), v8::Local<v8::Name>::Cast(key.data()));
    return result.IsJust() && result.ToChecked();
}

ScopedValue Object::Accessor::ownProperties() const{
    return ScopedValue(m_d->data->GetOwnPropertyNames());
}


// Object::ArrayAccessor implementation
// ----------------------------------------------------------------------

class Object::ArrayAccessorPrivate{
public:
    ArrayAccessorPrivate(const v8::Local<v8::Array>& o) : data(o){}

    v8::Local<v8::Array> data;
};


Object::ArrayAccessor::ArrayAccessor(Object o)
    : m_d(new Object::ArrayAccessorPrivate(v8::Local<v8::Array>::Cast(o.data()))){
}

Object::ArrayAccessor::ArrayAccessor(Engine *, const ScopedValue &lval)
    : m_d(new Object::ArrayAccessorPrivate(v8::Local<v8::Array>::Cast(lval.data())))
{
}

Object::ArrayAccessor::~ArrayAccessor(){
    delete m_d;
}

int Object::ArrayAccessor::length() const{
    return static_cast<int>(m_d->data->Length());
}

ScopedValue Object::ArrayAccessor::get(int index){
    return m_d->data->Get(static_cast<uint32_t>(index));
}

void Object::ArrayAccessor::set(int index, const ScopedValue &value){
    m_d->data->Set(static_cast<uint32_t>(index), value.data());
}


}}// namespace lv, el
