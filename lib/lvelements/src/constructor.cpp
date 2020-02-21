#include "constructor.h"
#include "live/elements/engine.h"
#include "live/elements/element.h"
#include "v8nowarnings.h"


namespace lv{ namespace el{

void Constructor::bindLifeTimeWithObject(Element *e, const v8::FunctionCallbackInfo<v8::Value> &info){
    e->setLifeTimeWithObject(info.This());
}

void Constructor::assignSelfReturnValue(const v8::FunctionCallbackInfo<v8::Value> &info){
    info.GetReturnValue().Set(info.This());
}

void Constructor::nullImplementation(const v8::FunctionCallbackInfo<v8::Value> &info){
    Engine* engine = reinterpret_cast<Engine*>(info.GetIsolate()->GetData(0));
    Exception e = CREATE_EXCEPTION(lv::Exception, "Class is not creatable.", Exception::toCode("~Constructor"));
    engine->throwError(&e, nullptr);
}

}} // namespace lv, el
