#include "property.h"
#include "live/elements/engine.h"
#include "live/elements/element.h"
#include "v8nowarnings.h"

namespace lv{ namespace el{

Property::Property(
        const std::string &name,
        const std::string &type,
        bool isWritable,
        const std::string &notify)
    : m_name(name)
    , m_type(type)
    , m_isWritable(isWritable)
    , m_notify(notify)
{
}

Property::~Property(){
}

void *Property::userData(const v8::PropertyCallbackInfo<void> &info){
    return info.Data().As<v8::External>()->Value();
}

Element *Property::that(const v8::PropertyCallbackInfo<void> &info){
    v8::Local<v8::Object> self = info.This();
    v8::Local<v8::External> wrap = v8::Local<v8::External>::Cast(self->GetInternalField(0));
    return reinterpret_cast<Element*>(wrap->Value());
}

Engine *Property::engine(const Element *element){
    return element->engine();
}

void Property::ptrGetImplementation(
        v8::Local<v8::Name>,
        const v8::PropertyCallbackInfo<v8::Value> &info)
{
    v8::Local<v8::Object> self = info.This();
    v8::Local<v8::External> wrap = v8::Local<v8::External>::Cast(self->GetInternalField(0));
    void* ptr = wrap->Value();
    Element* e = reinterpret_cast<Element*>(ptr);
    Property* p = reinterpret_cast<Property*>(info.Data().As<v8::External>()->Value());

    info.GetReturnValue().Set(p->read(e).data());
}

void Property::ptrSetImplementation(
        v8::Local<v8::Name>,
        v8::Local<v8::Value> value,
        const v8::PropertyCallbackInfo<void> &info)
{
    v8::Local<v8::Object> self = info.This();
    v8::Local<v8::External> wrap = v8::Local<v8::External>::Cast(self->GetInternalField(0));
    void* ptr = wrap->Value();
    Element* e = reinterpret_cast<Element*>(ptr);
    Property* p = reinterpret_cast<Property*>(info.Data().As<v8::External>()->Value());

    p->write(e, value);
}

void Property::ptrIndexGetImplementation(
        unsigned int index,
        const v8::PropertyCallbackInfo<v8::Value> &info)
{
    v8::Local<v8::Object> self = info.This();
    v8::Local<v8::External> wrap = v8::Local<v8::External>::Cast(self->GetInternalField(0));
    void* ptr = wrap->Value();
    Element* e = reinterpret_cast<Element*>(ptr);
    info.GetReturnValue().Set(e->typeMetaObject().indexGet()(e, static_cast<int>(index)).data());
}

void Property::ptrIndexSetterImplementation(
        unsigned int index,
        v8::Local<v8::Value> value,
        const v8::PropertyCallbackInfo<v8::Value> &info)
{
    v8::Local<v8::Object> self = info.This();
    v8::Local<v8::External> wrap = v8::Local<v8::External>::Cast(self->GetInternalField(0));
    void* ptr = wrap->Value();
    Element* e = reinterpret_cast<Element*>(ptr);
    e->typeMetaObject().indexSet()(e, static_cast<int>(index), LocalValue(value));
}

InstanceProperty::InstanceProperty(
        Element* e,
        const std::string &name,
        const std::string &type,
        const LocalValue &value,
        bool isWritable,
        const std::string &notify)
    : Property(name, type, isWritable, notify)
{
    m_value = value.toValue(e->engine());
}

InstanceProperty::~InstanceProperty(){
}

void InstanceProperty::write(Element *e, const v8::Local<v8::Value> &param){
    if ( !isWritable() )
        throw std::exception(); // TODO: Switch to engine exception
    m_value = LocalValue(Property::engine(e), param).toValue(e->engine());
    if ( !changedEvent().empty() ){
        Function::Parameters p(1);
        p.assign(0, param);
        e->trigger(changedEvent(), p);
    }
}

const Value &InstanceProperty::value() const{
    return m_value;
}

}} // namespace lv, el
