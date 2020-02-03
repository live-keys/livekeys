#ifndef LVERRORHANDLER_H
#define LVERRORHANDLER_H

#include "live/elements/lvelementsglobal.h"
#include "live/elements/element.h"

namespace lv{ namespace el{

class LV_ELEMENTS_EXPORT ErrorHandler : public Element{

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

    void rethrow(LocalValue e);

public:
    Event error(LocalValue data){
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
