#ifndef LVEVENT_H
#define LVEVENT_H

#include "live/elements/lvelementsglobal.h"
#include "live/elements/value.h"
#include "live/elements/function.h"
#include "live/elements/callable.h"

#include "live/meta/functionargs.h"
#include "live/meta/indextuple.h"

#include <functional>
#include <list>
#include <set>
#include <typeindex>
#include <map>

namespace lv{ namespace el{

class Element;
class Engine;

class LV_ELEMENTS_EXPORT Event{
public:
    typedef unsigned long long Id;
};

/**
 * @brief Generates Unique Event Ids for each event function.
 *
 * It first generates a unique Id for the actual class, then combines the generatesd
 * class id with the local event id for the function pointer from that class.
 *
 * The byte allocation per event id is:
 *  * 2 bytes for instance events
 *  * 2 bytes for type events
 *  * 4 bytes for type id
 *
 * i.e.
 *
 * @code
 * class A{ // A could have type Id 100
 *    Event someEvent(); // &A::someEvent could have a local id of 10, stored together
 *                       // with the type Id, where the type id is stored in the 4 least
 *                       // significant bytes, and the local id in the next 2 bytes
 * };
 * @endcode
 *
 */
class LV_ELEMENTS_EXPORT EventIdGenerator{

public:
    static std::map<std::type_index, Event::Id> registeredTypes;

    template<typename T>
    class TypeEventIdGenerator{

    public:
        typedef void (T::*EventFunction)();

        static std::list<std::pair<EventFunction, Event::Id> >& registeredEvents(){
            static std::list<std::pair<EventFunction, Event::Id> > internal;
            return internal;
        }

        template<typename ...Args>
        static Event::Id localEventId(Event(T::*f)(Args...)){

            // This is allowed according to Standard C: ISO/IEC 9899:1999(E) SECTION 6.3.2.3, paragraph 8
            EventFunction ei = reinterpret_cast<EventFunction>(f);
            Event::Id eId = 0;

            for ( auto it = registeredEvents().begin(); it != registeredEvents().end(); ++it ){
                if ( it->first == ei ){
                    return it->second;
                }
            }

            eId = registeredEvents().size();
            registeredEvents().push_back(std::make_pair(ei, eId));

            return eId;
        }

    };


    template<typename T>
    static Event::Id typeEventId(){
        std::type_index t(typeid(T));
        Event::Id tId = 0;

        auto it = EventIdGenerator::registeredTypes.find(t);
        if ( it == EventIdGenerator::registeredTypes.end() ){
            tId = static_cast<Event::Id>(EventIdGenerator::registeredTypes.size());
            EventIdGenerator::registeredTypes[t] = tId;
        } else {
            tId = it->second;
        }
        return tId;
    }


    template<typename T, typename ...Args>
    static Event::Id eventId(Event(T::*f)(Args...)){
        Event::Id tId = typeEventId<T>();
        Event::Id lId = TypeEventIdGenerator<T>::localEventId(f);
        return concatenate(tId, lId, 0);
    }

    static Event::Id concatenate(Event::Id typeId, Event::Id localEventId, Event::Id instanceEventId){
        return (instanceEventId << 48 ) | (localEventId << 32) | typeId;
    }
    static void separate(Event::Id eventId, Event::Id& typeId, Event::Id& localEventId, Event::Id& instanceEventId){
        typeId          = eventId & 0xFFFF;
        localEventId    = (eventId >> 32) & 0xFF;
        instanceEventId = (eventId >> 48);
    }


};

/**
 * @brief Event for an element instance.
 *
 * The code below will add an instance event to the 'e' object.
 *
 * @code
 * Element* e = new Element(nullptr);
 * Element::addEvent(e, "somewarning", ["int"]);
 * @endcode
 */
class LV_ELEMENTS_EXPORT InstanceEvent{

public:
    friend class Element;

    static void callbackImplementation(const v8::FunctionCallbackInfo<v8::Value>& info);

public:
    InstanceEvent(Element* parent, Event::Id eventId, int numArguments)
        : m_numArguments(numArguments)
        , m_parent(parent)
        , m_eventId(eventId)
    {}

    void call(const Function::Parameters& params);

private:
    int       m_numArguments;
    Element*  m_parent;
    Event::Id m_eventId;
};

/**
 * @brief Connection between an event emitter and a listener.
 */
class LV_ELEMENTS_EXPORT EventConnection{

public:
    EventConnection(Element* listener, Element* emitter, Event::Id eventId)
        : m_listener(listener)
        , m_emitter(emitter)
        , m_eventId(eventId)
    {}

    static void remove(EventConnection* conn);

    virtual ~EventConnection(){}

    Element* listener(){ return m_listener; }
    Element* emitter(){ return m_emitter; }
    Event::Id eventId(){ return m_eventId; }

private:
    Element*  m_listener;
    Element*  m_emitter;
    Event::Id m_eventId;
};

template<typename ...Args>
class EventConnectionFunction : public EventConnection{
public:
    EventConnectionFunction(
            Element* emitter,
            Event::Id eventId,
            const std::function<void(Args...)>& f)
        : EventConnection(nullptr, emitter, eventId)
        , m_f(f)
    {}

    EventConnectionFunction(
            Element* listener,
            Element* emitter,
            Event::Id eventId,
            const std::function<void(Args...)>& f)
        : EventConnection(listener, emitter, eventId)
        , m_f(f)
    {}

    ~EventConnectionFunction(){}

    void call(Args... args){
        // preserve function capture data in case the function gets deleted from within
        std::function<void(Args...)> fcopy = m_f;
        fcopy(args...);
    }

private:
    std::function<void(Args...)> m_f;
};


class LV_ELEMENTS_EXPORT EventListenerContainerBase{

public:
    template<typename ...Args, int... Indexes>
    static void callFunction(
            EventConnectionFunction<Args...>* f,
            meta::IndexTuple<Indexes...>,
            const Function::Parameters& params)
    {
        f->call(params.template getValue<
                Indexes, typename std::remove_const<typename std::remove_reference<Args>::type>::type>(elementEngine(f->emitter()))...);
    }

public:
    virtual void collectListeningElements(std::set<Element*>& elements) = 0;
    virtual void removeConnection(EventConnection* conn) = 0;
    virtual void removeConnections() = 0;
    virtual EventConnection* append(Element* emitter, Event::Id eventId, const Callable& f) = 0;
    virtual EventConnection* append(Element* emitter, Event::Id eid, std::function<void(const Function::Parameters&)> f) = 0;
    virtual void trigger(const Function::Parameters& params) = 0;
    virtual bool isRunning() const = 0;
    static Engine* elementEngine(Element* element);

    virtual ~EventListenerContainerBase(){}
};

template<typename ...Args>
class EventListenerContainer : public EventListenerContainerBase{
public:
    void call(Args... args){
        auto it = m_listeners.begin();
        m_callIterators.push_back(&it); // store the iterator in case the list changes

        while ( it != m_listeners.end() ){
            EventConnectionFunction<Args...>* f = *it;
            ++it; // advance the iterator, in case the list changes this can be updated to the new value
            f->call(args...);
        }
        m_callIterators.remove(&it);
    }

    virtual EventConnection* append(Element* emitter, Event::Id eid, const Callable& f) override{
        Engine* eng = elementEngine(emitter);
        EventConnectionFunction<Args...>* ecf = new EventConnectionFunction<Args...>( emitter, eid, [emitter, eng, f](Args ...args){
            f.call(emitter, Function::Parameters({ScopedValue::createValue<Args>(eng, args)...})); }
        );
        append(ecf);
        return ecf;
    }

    virtual EventConnection* append(Element* emitter, Event::Id eid, std::function<void(const Function::Parameters&)> f) override{
        Engine* eng = elementEngine(emitter);
        EventConnectionFunction<Args...>* ecf = new EventConnectionFunction<Args...>( emitter, eid, [emitter, eng, f](Args ...args){
            f(Function::Parameters({ScopedValue::createValue<Args>(eng, args)...})); }
        );
        append(ecf);
        return ecf;
    }

    void collectListeningElements(std::set<Element*>& elements) override{
        for ( auto f : m_listeners ){
            Element* e = f->listener();
            if ( e )
                elements.insert(e);
        }
    }

    bool isRunning() const override{ return !m_callIterators.empty(); }

    void removeConnection(EventConnection* conn) override{
        for ( auto it = m_listeners.begin(); it != m_listeners.end(); ++it ){
            if ( *it == conn ){
                // update all call iterators that will be deleted
                for ( auto callIt = m_callIterators.begin(); callIt != m_callIterators.end(); ++callIt ){
                    auto callItValue = *callIt;
                    if ( *callItValue == it ){
                        ++(*callItValue);
                    }
                }
                m_listeners.erase(it);

                delete conn;
                return;
            }
        }
    }

    virtual void removeConnections() override{
        for ( auto f : m_listeners ){
            delete f;
        }
        m_listeners.clear();
        for ( auto callIt = m_callIterators.begin(); callIt != m_callIterators.end(); ++callIt ){
            **callIt = m_listeners.end();
        }
    }

    void append(EventConnectionFunction<Args...>* ed){
        m_listeners.push_back(ed);
    }

    virtual void trigger(const Function::Parameters& params) override{
        auto it = m_listeners.begin();
        m_callIterators.push_back(&it); // store the iterator in case the list changes

        while ( it != m_listeners.end() ){
            EventConnectionFunction<Args...>* f = *it;
            ++it; // advance the iterator, in case the list changes this can be updated to the new value
            EventListenerContainerBase::callFunction(f, typename meta::make_indexes<Args...>::type(), params);
        }
        m_callIterators.remove(&it);
    }

    ~EventListenerContainer(){
        for ( auto f : m_listeners ){
            delete f;
        }
    }

    typedef std::list<EventConnectionFunction<Args...>* > ConnectionList;
    typedef typename ConnectionList::iterator ConnectionListIterator;
    typedef typename ConnectionList::iterator* ConnectionListIteratorPtr;

private:
    ConnectionList m_listeners;
    std::list<ConnectionListIteratorPtr> m_callIterators;
};

template<>
class EventListenerContainer<const Function::Parameters&> : public EventListenerContainerBase{
public:
    void call(const Function::Parameters& p){
        auto it = m_listeners.begin();
        m_callIterators.push_back(&it); // store the iterator in case the list changes

        while ( it != m_listeners.end() ){
            EventConnectionFunction<const Function::Parameters&>* f = *it;
            ++it; // advance the iterator, in case the list changes this can be updated to the new value
            f->call(p);
        }
        m_callIterators.remove(&it);
    }

    virtual EventConnection* append(Element* emitter, Event::Id eid, const Callable& f) override{
//        Engine* eng = elementEngine(emitter);

        EventConnectionFunction<const Function::Parameters&>* ecf =
            new EventConnectionFunction<const Function::Parameters&>( emitter, eid, [emitter, /*eng,*/ f](const Function::Parameters& p){
                f.call(emitter, p);
            });

        append(ecf);
        return ecf;
    }

    virtual EventConnection* append(Element* emitter, Event::Id eid, std::function<void(const Function::Parameters&)> f) override{
//        Engine* eng = elementEngine(emitter);
        EventConnectionFunction<const Function::Parameters&>* ecf =
            new EventConnectionFunction<const Function::Parameters&>( emitter, eid, [/*emitter, eng, */f](const Function::Parameters& p){
                f(p);
            });

        append(ecf);
        return ecf;
    }

    void collectListeningElements(std::set<Element*>& elements) override{
        for ( auto f : m_listeners ){
            Element* e = f->listener();
            if ( e )
                elements.insert(e);
        }
    }

    bool isRunning() const override{ return !m_callIterators.empty(); }

    void removeConnection(EventConnection* conn) override{
        for ( auto it = m_listeners.begin(); it != m_listeners.end(); ++it ){
            if ( *it == conn ){
                // update all call iterators that will be deleted
                for ( auto callIt = m_callIterators.begin(); callIt != m_callIterators.end(); ++callIt ){
                    auto callItValue = *callIt;
                    if ( *callItValue == it ){
                        ++(*callItValue);
                    }
                }
                m_listeners.erase(it);

                delete conn;
                return;
            }
        }
    }


    virtual void removeConnections() override{
        for ( auto f : m_listeners ){
            delete f;
        }
        m_listeners.clear();
        for ( auto callIt = m_callIterators.begin(); callIt != m_callIterators.end(); ++callIt ){
            **callIt = m_listeners.end();
        }
    }

    void trigger(const Function::Parameters &params) override{
        call(params);
    }

    void append(EventConnectionFunction<const Function::Parameters&>* ed){
        m_listeners.push_back(ed);
    }

    ~EventListenerContainer(){
        for ( auto f : m_listeners ){
            delete f;
        }
    }

private:
    std::list<EventConnectionFunction<const Function::Parameters&>* > m_listeners;
    std::list<std::list<EventConnectionFunction<const Function::Parameters&>* >::iterator*> m_callIterators;
};

}} // namespace lv, el

#endif // LVEVENT_H
