#ifndef LVELEMENT_H
#define LVELEMENT_H

#include "live/elements/lvelementsglobal.h"
#include "live/elements/engine.h"
#include "live/elements/value.h"
#include "live/elements/event.h"
#include "live/elements/metaobject.h"

namespace lv{ namespace el{

class ElementPrivate;

/**
 * @brief The Element class
 *
 * @section2 Memory Management
 *
 * Use the ref() and unref() functions to add references to an Element object
 * within c++. This will assure js won't be able to delete the object until
 * all references are cleared. When an obejcts c++ references are 0, then
 * js will assume ownership of the object, and the GC collector will decide
 * when the best time to remove the object will be.
 *
 * @note The js GC does not guarantee if and when an element object will
 * be deleted. Therefore elements with high memory consumtion (i.e. Buffers)
 * should rely on manual deletion if possible.
 *
 * To delete an object from C++, the ref() count should be higher than 0,
 * making sure the ownership does not reside on the JS side.
 *
 * A parent will automatically add a reference to its children, and remove
 * that reference once the parent has been destroyed.
 *
 * @note that setting a parent for an element, then removing that parent will
 * not delete the element.
 *
 * @code
 * Element* parent = new Element(engine);
 * Element* e = new Element(engine);
 * e->setParent(parent); // will now add a ref to 'e'
 * e->setParent(nullptr); // will now unref 'e', however, even if 'e' has 0 references
 *                        // it won't be deleted, nor the JS GC triggered
 * @endcode
 *
 * However, if we were to delete the parent, then all of it's children will
 * unref() and also be deleted or triggered through the GC if there are no
 * more references available.
 *
 * @code
 * Element* parent = new Element(engine);
 * Element* e = new Element(engine);
 * e->setParent(parent); // will now add a ref to 'e'
 * delete parent; // will unref 'e' and delete it or trigger the JS GC
 * @endcode
 *
 * The GC is triggered only if there is a reference of 'e' within the JS,
 * i.e. 'e' has passed throught he JS engine.
 */
class LV_ELEMENTS_EXPORT Element{

    BASE_META_OBJECT{
        META_OBJECT_DESCRIBE(Element)
            .constructor()
            .scriptMethod<void, ScopedValue, Callable>("on", &Element::on)
            .scriptMethod("setParent", &Element::setParent)
            .scriptProperty<Element*>("parent", &Element::parent)
            .scriptFunction("assignDefaultProperty", &Element::assignDefaultProperty)
            .scriptFunction<void, Element*, ScopedValue, ScopedValue>("addEvent", &Element::addEvent)
            .scriptFunction("addProperty", &Element::addProperty)
            .scriptFunction("assignPropertyExpression", &Element::assignPropertyExpression)
            .scriptFunction("assignId", &Element::assignId)
        META_OBJECT_CLOSE
    }

    friend class Engine;
    friend class ScopedValue;
    friend class ElementPrivate;
    friend class InstanceEvent;

    class BindableExpression;

public:
    Element(el::Engine* engine);
    virtual ~Element();

    Engine* engine() const;

    // Parent and Children
    // -------------------

    void setParent(Element* parent);
    Element* parent();
    size_t totalChildren() const;


    // Type Functionality
    // ------------------

    template<typename T> T* cast(){ return static_cast<T*>(this); } //TODO: Implement checks
    virtual void setLifeTimeWithObject(const v8::Local<v8::Object> &obj);

    // Memory Management
    // -----------------

    void ref();
    void unref();
    bool hasScriptReference() const;
    static void destroy(Element* e);

    // Events
    // ------

    template<typename T, typename ...Args, typename F>
    EventConnection* on(Event(T::*e)(Args...), const F& f);

    template<typename T, typename ...Args, typename O, typename F>
    EventConnection* on(Event(T::*e)(Args...), O* obj, const F &f);

    EventConnection* on(const std::string& key, std::function<void(const Function::Parameters&)> f);
    EventConnection* on(const std::string& key, Callable callback);
    void on(ScopedValue eventKey, Callable callback);

    void trigger(const std::string& eventKey, const Function::Parameters& params);

    static void addEvent(Element *e, const std::string& key, const std::vector<std::string>& types);
    static void addEvent(Element* e, ScopedValue eventKey, ScopedValue types);
    static void addProperty(Element* e, ScopedValue propertyName, ScopedValue propertyOptions);
    static void assignPropertyExpression(
        Element* e,
        ScopedValue propertyName,
        ScopedValue propertyExpression,
        ScopedValue bindings);
    static void assignDefaultProperty(Element* e, ScopedValue value);
    static void assignId(Element* e, const std::string& id);

    void clearPropertyBoundExpression(const std::string& propertyName);

    const std::string& getId() const;

    template<typename T, typename ...Args>
    static Event::Id eventId(Event(T::*)(Args...));

    void removeListeningConnection(EventConnection* conn);
    void removeListeners();
    void removeEmitters();

    // Properties
    // ----------

    virtual InstanceProperty *addProperty(
        const std::string& name,
        const std::string& type,
        ScopedValue value,
        bool isDefault,
        bool isWritable,
        const std::string& notifyEvent
    );
    const std::string& defaultProperty() const;
    Property* property(const std::string& name);
    Property* property(const std::string& name) const;
    bool hasProperty(const std::string& name) const;
    ScopedValue get(const std::string& name);
    void set(const std::string& name, const ScopedValue& value);

    // Ready
    // -----

    virtual void onReady(){}

protected:
    template<typename ...Args> Event notify(Event::Id eventId, Args... args);

    // C++ References
    int m_refs;

private:
    Event notifyFromInstance(Event::Id eventId, const Function::Parameters& params);
    void removeInstanceProperties();
    void removeBoundPropertyExpressions();
    void removeListenerConnectionDontNotify(EventConnection* conn);
    void removeEmitterConnectionDontNotify(EventConnection* conn);

    // Remove all connections that have the 'e' as emitter
    void removeEmitterDontNotify(Element* e);

    // Connections that listen to this object
    std::map<Event::Id, EventListenerContainerBase*> m_listeningConnections;
    // Connections that this objects listens to
    std::list<EventConnection*>                      m_emittingConnections;

    // Private data
    ElementPrivate* m_d;

};

template<typename T, typename ...Args>
Event::Id Element::eventId(Event(T::*e)(Args...)){
    return EventIdGenerator::eventId(e);
}

template<typename T, typename ...Args, typename F>
EventConnection* Element::on(Event(T::*e)(Args...), const F& f){
    Event::Id eid = eventId(e);
    EventListenerContainer<Args...>* container = nullptr;

    auto delegateIt = m_listeningConnections.find(eid);
    if ( delegateIt == m_listeningConnections.end() ){
        container = new EventListenerContainer<Args...>;
        m_listeningConnections[eid] = container;
    } else {
        container = static_cast<EventListenerContainer<Args...>*>(delegateIt->second);
    }

    EventConnectionFunction<Args...>* eventConn = new EventConnectionFunction<Args...>(this, eid, f);
    container->append(eventConn);

    return eventConn;
}

template<typename T, typename ...Args, typename O, typename F>
EventConnection* Element::on(Event(T::*e)(Args...), O* obj, const F &f){
    Event::Id eid = eventId(e);
    EventListenerContainer<Args...>* container = nullptr;

    auto delegateIt = m_listeningConnections.find(eid);
    if ( delegateIt == m_listeningConnections.end() ){
        container = new EventListenerContainer<Args...>;
        m_listeningConnections[eid] = container;
    } else {
        container = static_cast<EventListenerContainer<Args...>*>(delegateIt->second);
    }

    Element* el = static_cast<Element*>(obj);

    EventConnectionFunction<Args...>* eventConn = new EventConnectionFunction<Args...>(
        el, this, eid, [obj, f](Args ...args){ (obj->*f)(args...); }
    );
    container->append(eventConn);
    el->m_emittingConnections.push_back(eventConn);

    return eventConn;
}

template<typename ...Args>
Event Element::notify(Event::Id eventId, Args... args){
    auto delegateIt = m_listeningConnections.find(eventId);
    if ( delegateIt != m_listeningConnections.end() ){
        EventListenerContainer<Args...>* ed = static_cast<EventListenerContainer<Args...>*>(delegateIt->second);
        ed->call(args...);
    }
    return Event();
}

inline Event Element::notifyFromInstance(Event::Id eventId, const Function::Parameters &params){
    auto delegateIt = m_listeningConnections.find(eventId);
    if ( delegateIt != m_listeningConnections.end() ){
        EventListenerContainer<const Function::Parameters&>* ed = static_cast<EventListenerContainer<const Function::Parameters&>*>(delegateIt->second);
        ed->trigger(params);
    }
    return Event();
}

}} // namespace lv, el

#endif // LVELEMENT_H
