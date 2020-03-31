#ifndef LVTUPLE_H
#define LVTUPLE_H

#include "live/elements/element.h"

namespace lv{ namespace el{

class Tuple : public Element{

    META_OBJECT{
        META_OBJECT_DESCRIBE(Tuple)
            .base<Element>()
            .constructor()
            .scriptEvent("propertyChanged", &Tuple::propertyChanged)
        META_OBJECT_CLOSE
    }

public:
    Tuple(el::Engine* engine);
    ~Tuple();

    virtual InstanceProperty *addProperty(
        const std::string& name,
        const std::string& type,
        ScopedValue value,
        bool isDefault,
        bool isWritable,
        const std::string& notifyEvent
    ) override;

public:
    Event propertyChanged(const std::string& name){
        static Event::Id eid = eventId(&Tuple::propertyChanged); return notify(eid, name);
    }

private:

};

}}// namespace

#endif // LVTUPLE_H
