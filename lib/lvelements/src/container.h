#ifndef LVCONTAINER_H
#define LVCONTAINER_H

#include "live/elements/element.h"
#include "live/elements/list.h"

namespace lv{ namespace el{

class LV_ELEMENTS_EXPORT Container : public Element{

    META_OBJECT{
        META_OBJECT_DESCRIBE(Container)
            .constructor()
            .base<Element>()
            .scriptProperty<LocalValue>("children", &Container::children, &Container::setChildren, "childrenChanged")
            .scriptDefaultProperty("children")
            .scriptEvent("childrenChanged", &Container::childrenChanged)
        META_OBJECT_CLOSE
    }

public:
    Container(Engine* engine);
    ~Container();

    LocalValue children();
    void setChildren(LocalValue children);

    static LocalValue at(List* l, int index){
        return LocalValue(l->engine(), l->dataAs<std::vector<Element*>*>()->at(index));
    }
    static int length(List* l){
        return static_cast<int>(l->dataAs<std::vector<Element*>*>()->size());
    }
    static void assign(List* l, int index, LocalValue value){
        (*l->dataAs<std::vector<Element*>*>())[index] = value.toElement(l->engine());

    }
    static void append(List* l, LocalValue value){
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
