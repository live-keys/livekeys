#ifndef LVVALUE_P_H
#define LVVALUE_P_H


#include "value.h"
#include "v8nowarnings.h"

namespace lv{ namespace el{

// Point Type
// ----------

class Point{

public:
    static void constructPoint(const v8::FunctionCallbackInfo<v8::Value>& info){
        auto context = info.GetIsolate()->GetCurrentContext();
        double x = (info.Length() > 0) ? info[0]->ToNumber(context).ToLocalChecked()->Value() : 0;
        double y = (info.Length() > 1) ? info[1]->ToNumber(context).ToLocalChecked()->Value() : 0;

        auto setX = info.This()->Set(
            info.GetIsolate()->GetCurrentContext(),
            v8::String::NewFromUtf8(info.GetIsolate(), "x", v8::NewStringType::kInternalized).ToLocalChecked(),
            v8::Number::New(info.GetIsolate(), x)
        );
        setX.IsNothing();

        auto setY = info.This()->Set(
            info.GetIsolate()->GetCurrentContext(),
            v8::String::NewFromUtf8(info.GetIsolate(), "y", v8::NewStringType::kInternalized).ToLocalChecked(),
            v8::Number::New(info.GetIsolate(), y)
        );
        setY.IsNothing();

        info.GetReturnValue().Set(info.This());
    }

    static v8::Local<v8::FunctionTemplate> functionTemplate(v8::Isolate* isolate){
        v8::Local<v8::FunctionTemplate> localTpl;
        localTpl = v8::FunctionTemplate::New(isolate);
        localTpl->SetCallHandler(&Point::constructPoint);
        localTpl->SetClassName(v8::String::NewFromUtf8(isolate, "Point").ToLocalChecked());
        return localTpl;
    }

};

// Size Type
// ---------

class Size{

public:
    static void constructSize(const v8::FunctionCallbackInfo<v8::Value>& info){
        auto context = info.GetIsolate()->GetCurrentContext();
        double width = (info.Length() > 0) ? info[0]->ToNumber(context).ToLocalChecked()->Value() : 0;
        double height = (info.Length() > 1) ? info[1]->ToNumber(context).ToLocalChecked()->Value() : 0;

        auto setWidth = info.This()->Set(
            info.GetIsolate()->GetCurrentContext(),
            v8::String::NewFromUtf8(info.GetIsolate(), "width", v8::NewStringType::kInternalized).ToLocalChecked(),
            v8::Number::New(info.GetIsolate(), width)
        );
        setWidth.IsNothing();

        auto setHeight = info.This()->Set(
            info.GetIsolate()->GetCurrentContext(),
            v8::String::NewFromUtf8(info.GetIsolate(), "height", v8::NewStringType::kInternalized).ToLocalChecked(),
            v8::Number::New(info.GetIsolate(), height)
        );
        setHeight.IsNothing();

        info.GetReturnValue().Set(info.This());
    }

    static v8::Local<v8::FunctionTemplate> functionTemplate(v8::Isolate* isolate){
        v8::Local<v8::FunctionTemplate> localTpl;
        localTpl = v8::FunctionTemplate::New(isolate);
        localTpl->SetCallHandler(&Size::constructSize);
        localTpl->SetClassName(v8::String::NewFromUtf8(isolate, "Size").ToLocalChecked());
        return localTpl;
    }

};


// Rectangle Type
// --------------

class Rectangle{

public:
    static void constructRectangle(const v8::FunctionCallbackInfo<v8::Value>& info){
        auto context = info.GetIsolate()->GetCurrentContext();
        double x = (info.Length() > 0) ? info[0]->ToNumber(context).ToLocalChecked()->Value() : 0;
        double y = (info.Length() > 1) ? info[1]->ToNumber(context).ToLocalChecked()->Value() : 0;
        double width = (info.Length() > 2) ? info[2]->ToNumber(context).ToLocalChecked()->Value() : 0;
        double height = (info.Length() > 3) ? info[3]->ToNumber(context).ToLocalChecked()->Value() : 0;

        auto setX = info.This()->Set(
            info.GetIsolate()->GetCurrentContext(),
            v8::String::NewFromUtf8(info.GetIsolate(), "x", v8::NewStringType::kInternalized).ToLocalChecked(),
            v8::Number::New(info.GetIsolate(), x)
        );
        setX.IsNothing();

        auto setY = info.This()->Set(
            info.GetIsolate()->GetCurrentContext(),
            v8::String::NewFromUtf8(info.GetIsolate(), "y", v8::NewStringType::kInternalized).ToLocalChecked(),
            v8::Number::New(info.GetIsolate(), y)
        );
        setY.IsNothing();

        auto setWidth = info.This()->Set(
            info.GetIsolate()->GetCurrentContext(),
            v8::String::NewFromUtf8(info.GetIsolate(), "width", v8::NewStringType::kInternalized).ToLocalChecked(),
            v8::Number::New(info.GetIsolate(), width)
        );
        setWidth.IsNothing();

        auto setHeight = info.This()->Set(
            info.GetIsolate()->GetCurrentContext(),
            v8::String::NewFromUtf8(info.GetIsolate(), "height", v8::NewStringType::kInternalized).ToLocalChecked(),
            v8::Number::New(info.GetIsolate(), height)
        );
        setHeight.IsNothing();

        info.GetReturnValue().Set(info.This());
    }

    static v8::Local<v8::FunctionTemplate> functionTemplate(v8::Isolate* isolate){
        v8::Local<v8::FunctionTemplate> localTpl;
        localTpl = v8::FunctionTemplate::New(isolate);
        localTpl->SetCallHandler(&Rectangle::constructRectangle);
        localTpl->SetClassName(v8::String::NewFromUtf8(isolate, "Rectangle").ToLocalChecked());
        return localTpl;
    }

};

// Bind Error Function
// -------------------

inline void linkError(const v8::FunctionCallbackInfo<v8::Value>& info){
    if ( info.Length() != 2 ){
        info.GetIsolate()->ThrowException(
             v8::String::NewFromUtf8(info.GetIsolate(), "linkError requires 2 arguments to be provided.").ToLocalChecked());
        return;
    }

    v8::Local<v8::Object> errorObject   = info[0]->ToObject(info.GetIsolate()->GetCurrentContext()).ToLocalChecked();
    v8::Local<v8::Object> bindingObject = info[1]->ToObject(info.GetIsolate()->GetCurrentContext()).ToLocalChecked();

    v8::Maybe<bool> result = errorObject->Set(
        info.GetIsolate()->GetCurrentContext(),
        v8::String::NewFromUtf8(info.GetIsolate(), "object", v8::NewStringType::kInternalized).ToLocalChecked(),
        bindingObject
    );
    result.IsNothing();

    info.GetReturnValue().Set(errorObject);
}


}} // namespace lv, el


#endif // LVVALUE_P_H
