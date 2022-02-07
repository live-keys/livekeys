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

#ifndef LVELEMENT_P_H
#define LVELEMENT_P_H

#include "live/elements/value.h"
#include "live/elements/element.h"
#include "v8nowarnings.h"

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
        ScopedValue v = expression.call(element->engine(), Function::Parameters(0));
        property->write(element, v.data());
    }
};

class ElementPrivate{

public:
    static void weakElementDestructor(const v8::WeakCallbackInfo<Element>& data);
    static v8::Local<v8::Object> localObject(Element* element);
    static Element* elementFromObject(const v8::Local<v8::Object>& object);

    // Instance events
    std::map<std::string, InstanceEvent*>               instanceEvents;

    // Instance properties
    std::map<std::string, InstanceProperty*>            instanceProperties;

    // Bindable property expressions
    std::map<std::string, Element::BindableExpression*> boundPropertyExpressions;


    std::string                defaultProperty;
    std::string                id;

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
