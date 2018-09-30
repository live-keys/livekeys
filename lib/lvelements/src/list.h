#ifndef LVLIST_H
#define LVLIST_H

#include "live/elements/element.h"

namespace lv{ namespace el{

class LV_ELEMENTS_EXPORT List : public Element{

    META_OBJECT{
        META_OBJECT_DESCRIBE(List)
            .constructor()
            .base<Element>()
            .scriptProperty<int>("length", &List::length)
            .scriptIndexAccess(&List::indexGetter, &List::indexSetter)
            .scriptMethod("isWritable", &List::isWritable)
            .scriptMethod("at", &List::at)
            .scriptMethod("assign", &List::assign)
            .scriptMethod("append", &List::append)
            .scriptMethod("clear", &List::clear)
            .scriptEvent("valuesAdded", &List::valuesAdded)
            .scriptEvent("valuesRemoved", &List::valuesRemoved)
            .scriptEvent("valuesReset", &List::valuesReset)
        META_OBJECT_CLOSE
    }

public:
    List(Engine* engine);

    List(
        Engine* engine,
        void* data,
        const std::function<LocalValue(List*, int)>& at,
        const std::function<int(List*)>& length,
        bool isObservable
    );

    List(
        Engine* engine,
        void* data,
        const std::function<LocalValue(List*, int)>& at,
        const std::function<int(List*)>& length,
        const std::function<void(List*, int, LocalValue)>& assign,
        const std::function<void(List*, LocalValue)>& append,
        const std::function<void(List*)>& clear,
        bool isObservable = false
    );
    ~List();

    static LocalValue atImpl(List* l, int index);
    static int lengthImpl(List* l);

    void setDataDestructor(std::function<void(List*)> d);

    bool isWritable() const;
    int length();
    LocalValue at(int index);
    void assign(int index, const LocalValue &v);
    void append(const LocalValue& v);
    void clear();
    bool isObservable() const;

    static LocalValue indexGetter(Element* e, int index);
    static void indexSetter(Element* e, int index, LocalValue value);

    void* data();
    template<typename T> T dataAs(){ return reinterpret_cast<T>(m_data); }

public:
    Event valuesAdded(int from, int length);
    Event valuesRemoved(int from, int length);
    Event valuesReset();

private:
    void* m_data;
    bool  m_isObservable;

    std::function<LocalValue(List*, int)>       m_at;
    std::function<int(List*)>                   m_length;
    std::function<void(List*, int, LocalValue)> m_assign;
    std::function<void(List*, LocalValue)>      m_append;
    std::function<void(List*)>                  m_clear;
    std::function<void(List*)>                  m_dataDestructor;
};

inline void List::setDataDestructor(std::function<void (List *)> d){
    m_dataDestructor = d;
}

inline bool List::isWritable() const{
    if ( m_assign )
        return true;
    return false;
}

inline int List::length(){
    return m_length(this);
}

inline LocalValue List::at(int index){
    return m_at(this, index);
}

inline void List::assign(int index, const LocalValue& v){
    if (!m_assign )
        throw std::exception();
    m_assign(this, index, v);
}

inline void List::append(const LocalValue &v){
    if (!m_append)
        throw std::exception();
    m_append(this, v);
}

inline void List::clear(){
    if (!m_clear)
        throw std::exception();
    m_clear(this);
}

inline bool List::isObservable() const{
    return m_isObservable;
}

inline LocalValue List::indexGetter(Element *e, int index){
    return e->cast<List>()->at(index);
}

inline void List::indexSetter(Element *e, int index, LocalValue value){
    e->cast<List>()->assign(index, value);
}

inline void *List::data(){
    return m_data;
}

inline Event List::valuesAdded(int from, int length){
    static Event::Id eid = eventId(&List::valuesAdded);
    return notify(eid, from, length);
}

inline Event List::valuesRemoved(int from, int length){
    static Event::Id eid = eventId(&List::valuesRemoved);
    return notify(eid, from, length);
}

inline Event List::valuesReset(){
    static Event::Id eid = eventId(&List::valuesRemoved);
    return notify(eid);
}


}} // namespace lv, el

#endif // LIST_H
