#ifndef LVVALUE_P_H
#define LVVALUE_P_H


#include "value.h"
#include "v8.h"

namespace lv{ namespace el{

// Point Type
// ----------

class Point{

public:
    static void constructPoint(const v8::FunctionCallbackInfo<v8::Value>& info){
        double x = info.Length() > 0 ? info[0]->ToNumber(info.GetIsolate())->Value() : 0;
        double y = info.Length() > 1 ? info[1]->ToNumber(info.GetIsolate())->Value() : 0;

        info.This()->Set(
            v8::String::NewFromUtf8(info.GetIsolate(), "x", v8::String::kInternalizedString),
            v8::Number::New(info.GetIsolate(), x)
        );
        info.This()->Set(
            v8::String::NewFromUtf8(info.GetIsolate(), "y", v8::String::kInternalizedString),
            v8::Number::New(info.GetIsolate(), y)
        );
        info.GetReturnValue().Set(info.This());
    }

    static v8::Local<v8::FunctionTemplate> functionTemplate(v8::Isolate* isolate){
        v8::Local<v8::FunctionTemplate> localTpl;
        localTpl = v8::FunctionTemplate::New(isolate);
        localTpl->SetCallHandler(&Point::constructPoint);
        localTpl->SetClassName(v8::String::NewFromUtf8(isolate, "Point"));
        return localTpl;
    }

};

// Size Type
// ---------

class Size{

public:
    static void constructSize(const v8::FunctionCallbackInfo<v8::Value>& info){
        double width = info.Length() > 0 ? info[0]->ToNumber(info.GetIsolate())->Value() : 0;
        double height = info.Length() > 1 ? info[1]->ToNumber(info.GetIsolate())->Value() : 0;

        info.This()->Set(
            v8::String::NewFromUtf8(info.GetIsolate(), "width", v8::String::kInternalizedString),
            v8::Number::New(info.GetIsolate(), width)
        );
        info.This()->Set(
            v8::String::NewFromUtf8(info.GetIsolate(), "height", v8::String::kInternalizedString),
            v8::Number::New(info.GetIsolate(), height)
        );
        info.GetReturnValue().Set(info.This());
    }

    static v8::Local<v8::FunctionTemplate> functionTemplate(v8::Isolate* isolate){
        v8::Local<v8::FunctionTemplate> localTpl;
        localTpl = v8::FunctionTemplate::New(isolate);
        localTpl->SetCallHandler(&Size::constructSize);
        localTpl->SetClassName(v8::String::NewFromUtf8(isolate, "Size"));
        return localTpl;
    }

};


// Rectangle Type
// --------------

class Rectangle{

public:
    static void constructRectangle(const v8::FunctionCallbackInfo<v8::Value>& info){
        double x = info.Length() > 0 ? info[0]->ToNumber(info.GetIsolate())->Value() : 0;
        double y = info.Length() > 1 ? info[1]->ToNumber(info.GetIsolate())->Value() : 0;
        double width = info.Length() > 2 ? info[2]->ToNumber(info.GetIsolate())->Value() : 0;
        double height = info.Length() > 3 ? info[3]->ToNumber(info.GetIsolate())->Value() : 0;

        info.This()->Set(
            v8::String::NewFromUtf8(info.GetIsolate(), "x", v8::String::kInternalizedString),
            v8::Number::New(info.GetIsolate(), x)
        );
        info.This()->Set(
            v8::String::NewFromUtf8(info.GetIsolate(), "y", v8::String::kInternalizedString),
            v8::Number::New(info.GetIsolate(), y)
        );
        info.This()->Set(
            v8::String::NewFromUtf8(info.GetIsolate(), "width", v8::String::kInternalizedString),
            v8::Number::New(info.GetIsolate(), width)
        );
        info.This()->Set(
            v8::String::NewFromUtf8(info.GetIsolate(), "height", v8::String::kInternalizedString),
            v8::Number::New(info.GetIsolate(), height)
        );
        info.GetReturnValue().Set(info.This());
    }

    static v8::Local<v8::FunctionTemplate> functionTemplate(v8::Isolate* isolate){
        v8::Local<v8::FunctionTemplate> localTpl;
        localTpl = v8::FunctionTemplate::New(isolate);
        localTpl->SetCallHandler(&Rectangle::constructRectangle);
        localTpl->SetClassName(v8::String::NewFromUtf8(isolate, "Rectangle"));
        return localTpl;
    }

};

// Bind Error Function
// -------------------

inline void linkError(const v8::FunctionCallbackInfo<v8::Value>& info){
    if ( info.Length() != 2 ){
        info.GetIsolate()->ThrowException(
             v8::String::NewFromUtf8(info.GetIsolate(), "linkError requires 2 arguments to be provided."));
        return;
    }

    v8::Local<v8::Object> errorObject   = info[0]->ToObject(info.GetIsolate());
    v8::Local<v8::Object> bindingObject = info[1]->ToObject(info.GetIsolate());

    v8::Maybe<bool> result = errorObject->Set(
        info.GetIsolate()->GetCurrentContext(),
        v8::String::NewFromUtf8(info.GetIsolate(), "object", v8::String::kInternalizedString),
        bindingObject
    );
    result.IsNothing();

    info.GetReturnValue().Set(errorObject);
}


}} // namespace lv, el


#endif // LVVALUE_P_H
