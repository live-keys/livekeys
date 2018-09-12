#ifndef LVPROPERTY_H
#define LVPROPERTY_H

#include "live/elements/lvelementsglobal.h"
#include "live/elements/function.h"

namespace lv{ namespace el{

class LV_ELEMENTS_EXPORT Property{

public:
    friend class Element;

    typedef LocalValue(*IndexGetFunction)(Element*,int);
    typedef void(*IndexSetFunction)(Element*,int,LocalValue);

public:
    Property(
        const std::string& name,
        const std::string& type,
        bool isWritable,
        const std::string& notify = "");
    virtual ~Property();

protected:
    virtual LocalValue read(Element* e) = 0;
    virtual void write(Element* e, const v8::Local<v8::Value>& params) = 0;

public:
    static void* userData(const v8::PropertyCallbackInfo<void>& info);
    static Element* that(const v8::PropertyCallbackInfo<void>& info);
    static Engine* engine(const Element* element);

    static void ptrGetImplementation(
            v8::Local<v8::Name>,
            const v8::PropertyCallbackInfo<v8::Value> &info
    );
    static void ptrSetImplementation(
            v8::Local<v8::Name>,
            v8::Local<v8::Value> value,
            const v8::PropertyCallbackInfo<void>& info
    );

    static void ptrIndexGetImplementation(
            unsigned int index,
            const v8::PropertyCallbackInfo<v8::Value> &info);
    static void ptrIndexSetterImplementation(
            unsigned int index,
            v8::Local<v8::Value> value,
            const v8::PropertyCallbackInfo<v8::Value> &info);

    const std::string& type() const{ return m_type; }
    bool isWritable() const{ return m_isWritable; }
    const std::string& changedEvent() const{ return m_notify; }
    const std::string& name() const{ return m_name; }

private:
    std::string m_name;
    std::string m_type;
    bool        m_isWritable;
    std::string m_notify;

};

class LV_ELEMENTS_EXPORT InstanceProperty : public Property{

public:
    InstanceProperty(
        Element *e,
        const std::string& name,
        const std::string& type,
        const LocalValue& value,
        bool isWritable,
        const std::string& notify);
    ~InstanceProperty();

    LocalValue read(Element* e) override{ return LocalValue(Property::engine(e), m_value);}
    void write(Element* e, const v8::Local<v8::Value>& params) override;

    const Value& value() const;

private:
    Value       m_value;
};

template<class C, class T, class Getter, class Setter>
class SpecializedProperty : public Property{

public:
    SpecializedProperty(
            const std::string& name,
            const std::string& type,
            bool isWritable,
            const std::string& notify,
            Getter getter,
            Setter setter)
        : Property(name, type, isWritable, notify)
        , m_getter(getter)
        , m_setter(setter)
    {
    }

protected:
    virtual LocalValue read(Element* e){
        C* c = static_cast<C*>(e);
        return LocalValue(Property::engine(e), (c->*m_getter)());
    }

    virtual void write(Element* e, const v8::Local<v8::Value>& value){
        C* c = static_cast<C*>(e);
        T dd = convertFromV8<T>(Property::engine(e), value);
        (c->*m_setter)(dd);
    }

private:
    Getter m_getter;
    Setter m_setter;
};

}} // namespace lv, script

#endif // LVPROPERTY_H
