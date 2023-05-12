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

#ifndef LVCONTAINER_H
#define LVCONTAINER_H

#include "live/elements/element.h"
#include "live/elements/list.h"

namespace lv{ namespace el{

class LV_ELEMENTS_ENGINE_EXPORT Container : public Element{

    META_OBJECT{
        META_OBJECT_DESCRIBE(Container)
            .constructor()
            .base<Element>()
            .scriptProperty<ScopedValue>("children", &Container::children, &Container::setChildren, "childrenChanged")
            .scriptDefaultProperty("children")
            .scriptEvent("childrenChanged", &Container::childrenChanged)
        META_OBJECT_CLOSE
    }

public:
    Container(Engine* engine);
    ~Container() override;

    ScopedValue children();
    void setChildren(ScopedValue children);

    static ScopedValue at(List* l, int index){
        return ScopedValue(l->engine(), l->dataAs<std::vector<Element*>*>()->at(static_cast<size_t>(index)));
    }
    static int length(List* l){
        return static_cast<int>(l->dataAs<std::vector<Element*>*>()->size());
    }
    static void assign(List* l, int index, ScopedValue value){
        (*l->dataAs<std::vector<Element*>*>())[static_cast<size_t>(index)] = value.toElement(l->engine());

    }
    static void append(List* l, ScopedValue value){
        l->dataAs<std::vector<Element*>*>()->push_back(value.toElement(l->engine()));
    }
    static void clear(List* l){
        l->dataAs<std::vector<Element*>*>()->clear();
    }

    const std::vector<Element*> data() const;

public:
    Event childrenChanged();

private:
    List* createList();

    std::vector<Element*> m_data;
    List* m_children;
};

inline const std::vector<Element *> Container::data() const{
    return m_data;
}

inline Event Container::childrenChanged(){
    static Event::Id eid = eventId(&List::valuesRemoved);
    return notify(eid);
}

}} // namespace lv, el

#endif // CONTAINER_H
