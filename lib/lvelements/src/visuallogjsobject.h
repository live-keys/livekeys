#ifndef VISUALLOGJSOBJECT_H
#define VISUALLOGJSOBJECT_H

#include "element_p.h"
#include "live/visuallog.h"
#include "live/mlnode.h"
#include "live/elements/mlnodetojs.h"

namespace lv{ namespace el{

class VisualLogJsObject{

public:
    static void f(const v8::FunctionCallbackInfo<v8::Value>& info){
        logHelper(VisualLog::MessageInfo::Fatal, info);
    }

    static void e(const v8::FunctionCallbackInfo<v8::Value>& info){
        logHelper(VisualLog::MessageInfo::Error, info);
    }

    static void w(const v8::FunctionCallbackInfo<v8::Value>& info){
        logHelper(VisualLog::MessageInfo::Warning, info);
    }

    static void i(const v8::FunctionCallbackInfo<v8::Value>& info){
        logHelper(VisualLog::MessageInfo::Info, info);
    }

    static void d(const v8::FunctionCallbackInfo<v8::Value>& info){
        logHelper(VisualLog::MessageInfo::Debug, info);
    }

    static void v(const v8::FunctionCallbackInfo<v8::Value>& info){
        logHelper(VisualLog::MessageInfo::Verbose, info);
    }

    static void configure(const v8::FunctionCallbackInfo<v8::Value>& info){
        Engine* engine = reinterpret_cast<Engine*>(info.GetIsolate()->GetData(0));
        if ( info.Length() != 2 ){
            lv::Exception e = CREATE_EXCEPTION(lv::Exception, "Invalid number of arguments provided.", 0);
            engine->throwError(&e, nullptr);
            return;
        }

        v8::String::Utf8Value utfKey(info[0]->ToString(info.GetIsolate()));
        LocalValue val(info[1]);

        MLNode mlopt;
        ml::fromJs(val, mlopt, engine);
        vlog().configure(*utfKey, mlopt);
    }

    static void captureStackHelper(Engine* engine, VisualLog& vl){
        v8::Local<v8::StackTrace> st = v8::StackTrace::CurrentStackTrace(engine->isolate(), 1, v8::StackTrace::kScriptName);
        v8::Local<v8::StackFrame> sf = st->GetFrame(0);
        v8::String::Utf8Value scriptName(sf->GetScriptName());
        v8::String::Utf8Value functionName(sf->GetFunctionName());
        sf->GetLineNumber();

        vl.at(*scriptName, sf->GetLineNumber(), *functionName);
    }

    static void logHelper(VisualLog::MessageInfo::Level level, const v8::FunctionCallbackInfo<v8::Value>& info){

        bool captureStack = info.This()->Get(v8::String::NewFromUtf8(info.GetIsolate(), "sourceInfo"))->BooleanValue();

        if ( info.Length() == 0 ){
            VisualLog vl(level);
            if ( captureStack ){
                Engine* engine = reinterpret_cast<Engine*>(info.GetIsolate()->GetData(0));
                captureStackHelper(engine, vl);
            }

        } else if ( info.Length() == 1 ){
            VisualLog vl(level);
            if ( captureStack ){
                Engine* engine = reinterpret_cast<Engine*>(info.GetIsolate()->GetData(0));
                captureStackHelper(engine, vl);
            }

            v8::Local<v8::Value> arg = info[0];
            if ( arg->IsObject() ){
                v8::Local<v8::Object> vo = v8::Local<v8::Object>::Cast(arg);
                if ( vo->InternalFieldCount() > 0){
                    Element* e = ElementPrivate::elementFromObject(vo);
                    if ( e->typeMetaObject().isLoggable() ){
                        e->typeMetaObject().log(vl, e);
                        return;
                    }
                }
            }

            v8::String::Utf8Value utfVal(arg->ToString());
            vl << *utfVal;

        } else if ( info.Length() == 2 ){
            v8::String::Utf8Value utfKey(info[0]->ToString());

            VisualLog vl(*utfKey, level);
            if ( captureStack ){
                Engine* engine = reinterpret_cast<Engine*>(info.GetIsolate()->GetData(0));
                captureStackHelper(engine, vl);
            }

            v8::Local<v8::Value> arg = info[1];
            if ( arg->IsObject() ){
                v8::Local<v8::Object> vo = v8::Local<v8::Object>::Cast(arg);
                if ( vo->InternalFieldCount() > 0){
                    Element* e = ElementPrivate::elementFromObject(vo);
                    if ( e->typeMetaObject().isLoggable() ){
                        e->typeMetaObject().log(vl, e);
                        return;
                    }
                    return;
                }
            }

            v8::String::Utf8Value utfVal(arg->ToString());
            vl << *utfVal;

        } else {
            if ( info.Length() > 2 ){
                Engine* engine = reinterpret_cast<Engine*>(info.GetIsolate()->GetData(0));
                lv::Exception e = CREATE_EXCEPTION(lv::Exception, "Invalid number of arguments provided.", 0);
                engine->throwError(&e, nullptr);
                return;
            }
        }
    }

public:
    VisualLogJsObject();
};

}} // namespace lv, el

#endif // VISUALLOGJSOBJECT_H
