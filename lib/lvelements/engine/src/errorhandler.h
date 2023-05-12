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

#ifndef LVERRORHANDLER_H
#define LVERRORHANDLER_H

#include "live/elements/lvelengineglobal.h"
#include "live/elements/element.h"

namespace lv{ namespace el{

class LV_ELEMENTS_ENGINE_EXPORT ErrorHandler : public Element{

    META_OBJECT{
        META_OBJECT_DESCRIBE(ErrorHandler)
            .base<Element>()
            .constructor<>()
            .scriptProperty<Element*>("target", &ErrorHandler::target, &ErrorHandler::setTarget, "targetChanged")
            .scriptMethod("rethrow", &ErrorHandler::rethrow)
            .scriptEvent("error", &ErrorHandler::error)
            .scriptEvent("targetChanged", &ErrorHandler::targetChanged)
        META_OBJECT_CLOSE
    }

public:
    class ErrorData{
    public:
        std::string message;
        std::string stack;
        std::string fileName;
        int line;
    };

public:
    ErrorHandler(Engine* engine);
    ~ErrorHandler() override;

    void setTarget(Element* target);
    Element* target(){ return m_target; }

    void rethrow(ScopedValue e);

public:
    Event error(ScopedValue data){
        static Event::Id eid = eventId(&ErrorHandler::error);
        return notify(eid, data);
    }

    Event targetChanged(){
        static Event::Id eid = eventId(&ErrorHandler::targetChanged);
        return notify(eid);
    }

    void handlerError(const v8::Local<v8::Value>& exception, const ErrorData& ed);

private:
    Element*  m_target;
    ErrorData m_lastError;
};

}} // namespace lv, el

#endif // LVERRORHANDLER_H
