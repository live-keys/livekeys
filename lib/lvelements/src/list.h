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
        const std::function<ScopedValue(List*, int)>& at,
        const std::function<int(List*)>& length,
        bool isObservable
    );

    List(
        Engine* engine,
        void* data,
        const std::function<ScopedValue(List*, int)>& at,
        const std::function<int(List*)>& length,
        const std::function<void(List*, int, ScopedValue)>& assign,
        const std::function<void(List*, ScopedValue)>& append,
        const std::function<void(List*)>& clear,
        bool isObservable = false
    );
    ~List();

    static ScopedValue atImpl(List* l, int index);
    static int lengthImpl(List* l);

    void setDataDestructor(std::function<void(List*)> d);

    bool isWritable() const;
    int length();
    ScopedValue at(int index);
    void assign(int index, const ScopedValue &v);
    void append(const ScopedValue& v);
    void clear();
    bool isObservable() const;

    static ScopedValue indexGetter(Element* e, int index);
    static void indexSetter(Element* e, int index, ScopedValue value);

    void* data();
    template<typename T> T dataAs(){ return reinterpret_cast<T>(m_data); }

public:
    Event valuesAdded(int from, int length);
    Event valuesRemoved(int from, int length);
    Event valuesReset();

private:
    void* m_data;
    bool  m_isObservable;

    std::function<ScopedValue(List*, int)>       m_at;
    std::function<int(List*)>                   m_length;
    std::function<void(List*, int, ScopedValue)> m_assign;
    std::function<void(List*, ScopedValue)>      m_append;
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

inline ScopedValue List::at(int index){
    return m_at(this, index);
}

inline void List::assign(int index, const ScopedValue& v){
    if (!m_assign)
    {
        auto exc = CREATE_EXCEPTION(lv::Exception, "Assignment function doesn't exist", lv::Exception::toCode("~List"));
        engine()->throwError(&exc, nullptr);
    }
    m_assign(this, index, v);
}

inline void List::append(const ScopedValue &v){
    if (!m_append)
    {
        auto exc = CREATE_EXCEPTION(lv::Exception, "Append function doesn't exist", lv::Exception::toCode("~List"));
        engine()->throwError(&exc, nullptr);
    }
    m_append(this, v);
}

inline void List::clear(){
    if (!m_clear)
    {
        auto exc = CREATE_EXCEPTION(lv::Exception, "Clearing function doesn't exist", lv::Exception::toCode("~List"));
        engine()->throwError(&exc, nullptr);
    }
    m_clear(this);
}

inline bool List::isObservable() const{
    return m_isObservable;
}

inline ScopedValue List::indexGetter(Element *e, int index){
    return e->cast<List>()->at(index);
}

inline void List::indexSetter(Element *e, int index, ScopedValue value){
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
