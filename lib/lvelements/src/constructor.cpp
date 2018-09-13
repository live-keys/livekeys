#include "constructor.h"
#include "live/elements/engine.h"
#include "live/elements/element.h"
#include "v8.h"


namespace lv{ namespace el{


void Constructor::bindLifeTimeWithObject(Element *e, const v8::FunctionCallbackInfo<v8::Value> &info){
    e->setLifeTimeWithObject(info.This());
}

void Constructor::assignSelfReturnValue(const v8::FunctionCallbackInfo<v8::Value> &info){
    info.GetReturnValue().Set(info.This());
}


}} // namespace lv, script
