#ifndef LVELEMENT_P_H
#define LVELEMENT_P_H

#include "live/elements/value.h"
#include "live/elements/element.h"
#include "v8.h"

namespace lv{ namespace el{


class Element::BindableExpression{
public:
    Callable         expression;
    std::string      propertyName;
    Element*         element;
    Property*        property;
    EventConnection* monitoringConnection;
    std::vector<EventConnection*> bindables;
    bool             isActive;

    BindableExpression(const Callable& exp) : expression(exp){}

    void run(){
        isActive = true;
        LocalValue v = expression.call(element->engine(), Function::Parameters(0));
        property->write(element, v.data());
    }
};

class ElementPrivate{

public:
    static void weakElementDestructor(const v8::WeakCallbackInfo<Element>& data);
    static v8::Local<v8::Object> localObject(Element* element);

    // Instance events
    std::map<std::string, InstanceEvent*>               instanceEvents;

    // Instance properties
    std::map<std::string, InstanceProperty*>            instanceProperties;

    // Bindable property expressions
    std::map<std::string, Element::BindableExpression*> boundPropertyExpressions;


    std::string                defaultProperty;

    // Children
    std::list<Element*>        children;
    Element*                   parent;

    Engine*                    engine;
    v8::Persistent<v8::Object> persistent;
};

inline void ElementPrivate::weakElementDestructor(const v8::WeakCallbackInfo<Element> &data) {
    Element* e = data.GetParameter();
    e->m_d->persistent.Reset();
    delete e;
}

}} // namespace lv, script

#endif // LVELEMENT_P_H
