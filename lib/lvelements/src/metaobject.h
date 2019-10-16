#ifndef LVMETAOBJECT_H
#define LVMETAOBJECT_H

#include "live/exception.h"
#include "live/mlnode.h"
#include "live/visuallog.h"
#include "live/elements/lvelementsglobal.h"
#include "live/elements/function.h"
#include "live/elements/method.h"
#include "live/elements/constructor.h"
#include "live/elements/property.h"
#include "live/elements/event.h"
#include "live/elements/eventfunction.h"

#include <string>
#include <functional>
#include <map>

namespace lv{ namespace el{

class Engine;

/**
 * @brief The MetaObject class
 */
class LV_ELEMENTS_EXPORT MetaObject{

public:
    typedef size_t Id;

    template<typename C>
    class Builder{

        friend class MetaObject;

    public:
        Builder(const std::string& name);

        template<typename T>
        Builder& base(){
            m_base = &T::metaObject();
            return *this;
        }

        // Constructor Management
        // ----------------------

        template<typename ...Args>
        Builder& constructor(){
            m_constructor = new ConcreteConstructor<C, Args...>();
            return *this;
        }

        Builder& scriptIndexAccess(Property::IndexGetFunction indexGet, Property::IndexSetFunction indexSet)
        {
            m_indexGet = indexGet;
            m_indexSet = indexSet;
            return *this;
        }

        // Methods Management
        // ------------------

        template<typename RT, typename ...Args>
        Builder& scriptMethod(const std::string& name, RT(C::*f)(Args...) ){
            auto it = m_methods.find(name);
            if ( it == m_methods.end() ){
                m_methods[name] = new Method<C, RT, Args...>(f);
            } else {
                THROW_EXCEPTION(lv::Exception, "Function \'" + name + "\' has already been defined for this class", 1);
            }
            return *this;
        }

        template<typename RT, typename ...Args>
        Builder& scriptMethod(const std::string& name, RT(C::*f)(Args...) const ){
            auto it = m_methods.find(name);
            if ( it == m_methods.end() ){
                m_methods[name] = new Method<C, RT, Args...>(f);
            } else {
                THROW_EXCEPTION(lv::Exception, "Function \'" + name + "\' has already been defined for this class", 1);
            }
            return *this;
        }

        // Functions Management
        // --------------------

        template<typename RT, typename ...Args>
        Builder& scriptFunction(const std::string& name, RT(*f)(Args...)){
            auto it = m_functions.find(name);
            if ( it == m_functions.end() ){
                Function* pf = new PointerFunction<RT, Args...>(f);
                m_functions[name] = pf;
            } else {
                THROW_EXCEPTION(lv::Exception, "Static function \'" + name + "\' has already been defined for this class", 1);
            }
            return *this;
        }

        // Property Management
        // -------------------

        // Write Properties

        template<typename T>
        Builder& scriptProperty(const std::string& name, T(C::*getter)(), void(C::*setter)(T), const std::string& notify){
            m_properties[name] = new SpecializedProperty<C, T, T(C::*)(), void(C::*)(T)>(
                name, typeid(T).name(), true, notify, getter, setter
            );
            return *this;
        }

        template<typename T>
        Builder& scriptProperty(const std::string& name, T(C::*getter)() const, void(C::*setter)(T), const std::string& notify){
            m_properties[name] = new SpecializedProperty<C, T, T(C::*)() const, void(C::*)(T)>(
                name, typeid(T).name(), true, notify, getter, setter
            );
            return *this;
        }

        template<typename T>
        Builder& scriptProperty(const std::string& name, const T&(C::*getter)(), void(C::*setter)(T), const std::string& notify){
            m_properties[name] = new SpecializedProperty<C, T, const T&(C::*)(), void(C::*)(T)>(
                name, typeid(T).name(), true, notify, getter, setter
            );
            return *this;
        }

        template<typename T>
        Builder& scriptProperty(const std::string& name, const T&(C::*getter)() const, void(C::*setter)(T), const std::string& notify){
            m_properties[name] = new SpecializedProperty<C, T, const T&(C::*)() const, void(C::*)(T)>(
                name, typeid(T).name(), true, notify, getter, setter
            );
            return *this;
        }

        // Write const properties

        template<typename T>
        Builder& scriptProperty(const std::string& name, T(C::*getter)(), void(C::*setter)(const T&), const std::string& notify){
            m_properties[name] = new SpecializedProperty<C, T, T(C::*)(), void(C::*)(const T&)>(
                name, typeid(T).name(), true, notify, getter, setter
            );
            return *this;
        }

        template<typename T>
        Builder& scriptProperty(const std::string& name, T(C::*getter)() const, void(C::*setter)(const T&), const std::string& notify){
            m_properties[name] = new SpecializedProperty<C, T, T(C::*)() const, void(C::*)(const T&)>(
                name, typeid(T).name(), true, notify, getter, setter
            );
            return *this;
        }

        template<typename T>
        Builder& scriptProperty(const std::string& name, const T&(C::*getter)(), void(C::*setter)(const T&), const std::string& notify){
            m_properties[name] = new SpecializedProperty<C, T, const T&(C::*)(), void(C::*)(const T&)>(
                name, typeid(T).name(), true, notify, getter, setter
            );
            return *this;
        }

        template<typename T>
        Builder& scriptProperty(const std::string& name, const T&(C::*getter)() const, void(C::*setter)(const T&), const std::string& notify){
            m_properties[name] = new SpecializedProperty<C, T, const T&(C::*)() const, void(C::*)(const T&)>(
                name, typeid(T).name(), true, notify, getter, setter
            );
            return *this;
        }

        // Read only properties

        template<typename T>
        Builder& scriptProperty(const std::string& name, T(C::*getter)(), const std::string& notify = ""){
            m_properties[name] = new SpecializedProperty<C, T, T(C::*)(), void(C::*)(T)>(
                name, typeid(T).name(), false, notify, getter, nullptr
            );
            return *this;
        }

        template<typename T>
        Builder& scriptProperty(const std::string& name, T(C::*getter)() const, const std::string& notify = ""){
            m_properties[name] = new SpecializedProperty<C, T, T(C::*)() const, void(C::*)(T)>(
                name, typeid(T).name(), true, notify, getter, nullptr
            );
            return *this;
        }

        template<typename T>
        Builder& scriptProperty(const std::string& name, const T&(C::*getter)(), const std::string& notify = ""){
            m_properties[name] = new SpecializedProperty<C, T, const T&(C::*)(), void(C::*)(T)>(
                name, typeid(T).name(), true, notify, getter, nullptr
            );
            return *this;
        }

        template<typename T>
        Builder& scriptProperty(const std::string& name, const T&(C::*getter)() const, const std::string& notify = ""){
            m_properties[name] = new SpecializedProperty<C, T, const T&(C::*)() const, void(C::*)(T)>(
                name, typeid(T).name(), true, notify, getter, nullptr
            );
            return *this;
        }

        Builder& scriptDefaultProperty(const std::string& name){
            if ( name.empty() )
                THROW_EXCEPTION(lv::Exception, "Default property name is empty.", 1);
            if ( !m_defaultProperty.empty() )
                THROW_EXCEPTION(lv::Exception, "Default property \'" + name + "\' is defined twice", 1);
            m_defaultProperty = name;
            return *this;
        }

        // Event Management
        // ----------------

        template<typename ...Args>
        Builder& scriptEvent(const std::string& name, Event(C::*f)(Args...) ){
            Event::Id eid = EventIdGenerator::eventId(f);
            EventFunction* ef = new ConcreteEventFunction<C, Args...>(eid, f);

            m_events[name]    = ef;
            m_eventsById[eid] = ef;

            return *this;
        }

        template<typename ...Args>
        Builder& scriptEvent(const std::string& name, Event(C::*f)(Args...) const ){
            Event::Id eid = EventIdGenerator::eventId(f);
            EventFunction* ef = new ConcreteEventFunction<C, Args...>(eid, f);

            m_events[name]    = ef;
            m_eventsById[eid] = ef;

            return *this;
        }

        // Serialization
        // -------------

        Builder& serialization(std::function<void(const C &, MLNode &)> srl){
            m_serialize = [srl](const Element* obj, lv::MLNode& node){
                const C* objtype = static_cast<const C*>(obj);
                srl(*objtype, node);
            };
        }

        Builder& deserialization(std::function<void(const MLNode&, C&)> dsrl){
            m_deserialize = [dsrl](const lv::MLNode& node, Element* obj){
                C* objtype = static_cast<C*>(obj);
                dsrl(node, *objtype);
            };
        }

        Builder& logging(std::function<void(lv::VisualLog&, const C&)> logger){
            m_log = [logger](lv::VisualLog& vl, const Element* obj){
                const C* objT = static_cast<C*>(obj);
                logger(vl, objT);
            };
        }

    private:
        std::string                           m_name;
        const MetaObject*                     m_base;
        MetaObject::Id                        m_typeId;
        std::map<std::string, Function*>      m_methods;
        std::map<std::string, Function*>      m_functions;
        std::map<std::string, EventFunction*> m_events;
        std::map<Event::Id,   EventFunction*> m_eventsById;
        std::map<std::string, Property*>      m_properties;
        std::string                           m_defaultProperty;
        Constructor*                          m_constructor;
        Property::IndexGetFunction            m_indexGet;
        Property::IndexSetFunction            m_indexSet;

        std::function<void(const Element*, lv::MLNode& node)>  m_serialize;
        std::function<void(const lv::MLNode& node, Element*)>  m_deserialize;
        std::function<void(lv::VisualLog& vl, const Element*)> m_log;
    };

public:
    MetaObject();

    const std::string& name() const{ return m_name; }

    template<typename T>
    static MetaObject fromBuilder(const MetaObject::Builder<T>& builder){
        MetaObject mo;
        mo.m_name          = builder.m_name;
        mo.m_base          = builder.m_base;
        mo.m_typeId        = builder.m_typeId;
        mo.m_functions     = builder.m_functions;
        mo.m_constructor   = builder.m_constructor;
        mo.m_ownMethods    = builder.m_methods;
        mo.m_ownEvents     = builder.m_events;
        mo.m_ownEventsById = builder.m_eventsById;
        mo.m_ownProperties = builder.m_properties;

        mo.m_methods       = builder.m_methods;
        mo.m_events        = builder.m_events;
        mo.m_eventsById    = builder.m_eventsById;
        mo.m_properties    = builder.m_properties;

        mo.m_indexGet      = builder.m_indexGet;
        mo.m_indexSet      = builder.m_indexSet;

        mo.m_defaultProperty = builder.m_defaultProperty;

        mo.m_serialize    = builder.m_serialize;
        mo.m_deserialize  = builder.m_deserialize;
        mo.m_log          = builder.m_log;

        if ( mo.m_base ){
            mo.m_methods.insert(mo.m_base->methodsBegin(), mo.m_base->methodsEnd());
            mo.m_events.insert(mo.m_base->eventsBegin(), mo.m_base->eventsEnd());
            mo.m_eventsById.insert(mo.m_base->m_eventsById.begin(), mo.m_base->m_eventsById.end());
            mo.m_properties.insert(mo.m_base->propertiesBegin(), mo.m_base->propertiesEnd());
            if ( mo.m_defaultProperty.empty() )
                mo.m_defaultProperty = mo.m_base->m_defaultProperty;
        }

        if ( !mo.m_defaultProperty.empty() && mo.m_properties.find(mo.m_defaultProperty) == mo.m_properties.end() )
            THROW_EXCEPTION(lv::Exception, "Default property \'" + mo.m_defaultProperty + "\' is not a class property.", 1);

        return mo;
    }

    template<typename T, typename ...Args>
    static constexpr Event::Id eventId(Event(T::*e)(Args...)){
        return EventIdGenerator::eventId(e);
    }

    Constructor* constructor() const { return m_constructor; }
    const MetaObject* base() const;


    size_t totalFunctions() const{ return m_functions.size(); }
    Function* getFunction(const std::string& name) const{ return m_functions.at(name); }
    std::map<std::string, Function*>::const_iterator functionsBegin() const;
    std::map<std::string, Function*>::const_iterator functionsEnd() const;


    size_t ownTotalProperties() const{ return m_ownProperties.size(); }
    std::map<std::string, Property*>::const_iterator ownPropertiesBegin() const;
    std::map<std::string, Property*>::const_iterator ownPropertiesEnd() const;

    size_t totalProperties() const{ return m_properties.size(); }
    std::map<std::string, Property*>::const_iterator propertiesBegin() const;
    std::map<std::string, Property*>::const_iterator propertiesEnd() const;

    size_t ownTotalEvents() const{ return m_events.size(); }
    std::map<std::string, EventFunction*>::const_iterator eventsBegin() const;
    std::map<std::string, EventFunction*>::const_iterator eventsEnd() const;

    size_t totalEvents() const{ return m_ownEvents.size(); }
    std::map<std::string, EventFunction*>::const_iterator ownEventsBegin() const;
    std::map<std::string, EventFunction*>::const_iterator ownEventsEnd() const;


    size_t ownTotalMethods() const{ return m_ownMethods.size(); }
    std::map<std::string, Function*>::const_iterator ownMethodsBegin() const;
    std::map<std::string, Function*>::const_iterator ownMethodsEnd() const;

    size_t totalMethods() const{ return m_methods.size(); }
    std::map<std::string, Function*>::const_iterator methodsBegin() const;
    std::map<std::string, Function*>::const_iterator methodsEnd() const;

    EventFunction* getEvent(const std::string& name) const;
    Property* getProperty(const std::string& name) const;
    Function* getMethod(const std::string& name) const;

    bool hasIndexAccess() const;
    Property::IndexGetFunction indexGet() const;
    Property::IndexSetFunction indexSet() const;

    const std::string& defaultProperty() const;

    bool isSerializable() const;
    bool isDeserializable() const;
    bool isLoggable() const;

    void serialize(const Element* object, lv::MLNode& node) const;
    void deserialize(const lv::MLNode& node, Element* object) const;
    void log(lv::VisualLog& vl, const Element* object) const;

private:
    std::string                           m_name;
    const MetaObject*                     m_base;
    MetaObject::Id                        m_typeId;
    std::map<std::string, Function*>      m_functions;
    Constructor*                          m_constructor;

    std::map<std::string, Function*>      m_methods;
    std::map<std::string, EventFunction*> m_events;
    std::map<Event::Id,   EventFunction*> m_eventsById;
    std::map<std::string, Property*>      m_properties;

    std::map<std::string, Function*>      m_ownMethods;
    std::map<std::string, EventFunction*> m_ownEvents;
    std::map<Event::Id,   EventFunction*> m_ownEventsById;
    std::map<std::string, Property*>      m_ownProperties;

    Property::IndexGetFunction            m_indexGet;
    Property::IndexSetFunction            m_indexSet;

    std::string                           m_defaultProperty;

    std::function<void(const Element*, MLNode& node)>  m_serialize;
    std::function<void(const MLNode& node, Element*)>  m_deserialize;
    std::function<void(lv::VisualLog& vl, const Element*)> m_log;
};

// Builder Implementation
// -----------------------------------------------------------------------------------

template<typename C>
MetaObject::Builder<C>::Builder(const std::string &name)
    : m_name(name)
    , m_base(nullptr)
    , m_constructor(nullptr)
    , m_indexGet(nullptr)
    , m_indexSet(nullptr)
{
    m_typeId = typeid(C).hash_code();
}


inline const MetaObject *MetaObject::base() const{
    return m_base;
}

inline std::map<std::string, Function*>::const_iterator MetaObject::functionsBegin() const{
    return m_functions.begin();
}

inline std::map<std::string, Function*>::const_iterator MetaObject::functionsEnd() const{
    return m_functions.end();
}

inline std::map<std::string, Property*>::const_iterator MetaObject::ownPropertiesBegin() const{
    return m_ownProperties.begin();
}

inline std::map<std::string, Property*>::const_iterator MetaObject::ownPropertiesEnd() const{
    return m_ownProperties.end();
}

inline std::map<std::string, Property*>::const_iterator MetaObject::propertiesBegin() const{
    return m_properties.begin();
}

inline std::map<std::string, Property*>::const_iterator MetaObject::propertiesEnd() const{
    return m_properties.end();
}

inline std::map<std::string, Function*>::const_iterator MetaObject::methodsBegin() const{
    return m_methods.begin();
}

inline std::map<std::string, Function*>::const_iterator MetaObject::methodsEnd() const{
    return m_methods.end();
}

inline std::map<std::string, Function*>::const_iterator MetaObject::ownMethodsBegin() const{
    return m_ownMethods.begin();
}

inline std::map<std::string, Function*>::const_iterator MetaObject::ownMethodsEnd() const{
    return m_ownMethods.end();
}

inline std::map<std::string, EventFunction*>::const_iterator MetaObject::eventsBegin() const{
    return m_events.begin();
}

inline std::map<std::string, EventFunction*>::const_iterator MetaObject::eventsEnd() const{
    return m_events.end();
}

inline std::map<std::string, EventFunction*>::const_iterator MetaObject::ownEventsBegin() const{
    return m_ownEvents.begin();
}

inline std::map<std::string, EventFunction*>::const_iterator MetaObject::ownEventsEnd() const{
    return m_ownEvents.end();
}


inline Property *MetaObject::getProperty(const std::string &name) const{
    auto it = m_properties.find(name);
    if ( it == m_properties.end() )
        return 0;
    return it->second;
}

inline Function *MetaObject::getMethod(const std::string &name) const{
    auto it = m_methods.find(name);
    if ( it == m_methods.end() )
        return 0;
    return it->second;
}

inline bool MetaObject::hasIndexAccess() const{
    return m_indexSet != nullptr && m_indexSet != nullptr;
}

inline Property::IndexGetFunction MetaObject::indexGet() const{
    return m_indexGet;
}

inline Property::IndexSetFunction MetaObject::indexSet() const{
    return m_indexSet;
}

inline EventFunction *MetaObject::getEvent(const std::string &name) const{
    auto it = m_events.find(name);
    if ( it == m_events.end() )
        return nullptr;
    return it->second;
}

inline const std::string &MetaObject::defaultProperty() const{
    return m_defaultProperty;
}

inline bool MetaObject::isSerializable() const{
    return m_serialize ? true : false;
}

inline bool MetaObject::isDeserializable() const{
    return m_deserialize ? true : false;
}

inline bool MetaObject::isLoggable() const{
    return m_log ? true : false;
}

inline void MetaObject::serialize(const Element *object, MLNode &node) const{
    m_serialize(object, node);
}

inline void MetaObject::deserialize(const MLNode &node, Element *object) const{
    m_deserialize(node, object);
}

inline void MetaObject::log(VisualLog &vl, const Element *object) const{
    m_log(vl, object);
}

}} // namespace lv, namespace script



// Meta object Macros
// -----------------------------------------------------------------------------------

#define META_OBJECT \
public: \
    virtual const MetaObject& typeMetaObject() const { return metaObject(); } \
    public:static const lv::el::MetaObject& metaObject()

#define META_OBJECT_DESCRIBE(_name) \
        static lv::el::MetaObject mo = lv::el::MetaObject::fromBuilder(MetaObject::Builder<_name>(#_name)

#define META_OBJECT_CLOSE ); return mo;


#endif // LVMETAOBJECT_H
