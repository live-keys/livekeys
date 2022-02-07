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

        v8::String::Utf8Value utfKey(info.GetIsolate(), info[0]->ToString(info.GetIsolate()->GetCurrentContext()).ToLocalChecked());
        ScopedValue val(info[1]);

        MLNode mlopt;
        ml::fromJs(val, mlopt, engine);
        vlog().configure(*utfKey, mlopt);
    }

    static void captureStackHelper(Engine* engine, VisualLog& vl){
        v8::Local<v8::StackTrace> st = v8::StackTrace::CurrentStackTrace(engine->isolate(), 1, v8::StackTrace::kScriptName);
        v8::Local<v8::StackFrame> sf = st->GetFrame(engine->isolate(), 0);
        v8::String::Utf8Value scriptName(engine->isolate(), sf->GetScriptName());
        v8::String::Utf8Value functionName(engine->isolate(), sf->GetFunctionName());
        sf->GetLineNumber();

        vl.at(*scriptName, sf->GetLineNumber(), *functionName);
    }

    static void logHelper(VisualLog::MessageInfo::Level level, const v8::FunctionCallbackInfo<v8::Value>& info){

        bool captureStack = info.This()->Get(
            info.GetIsolate()->GetCurrentContext(),
            v8::String::NewFromUtf8(info.GetIsolate(), "sourceInfo").ToLocalChecked()
        ).ToLocalChecked()->BooleanValue(info.GetIsolate());

        Engine* engine = reinterpret_cast<Engine*>(info.GetIsolate()->GetData(0));

        if ( info.Length() == 0 ){
            VisualLog vl(level);
            if ( captureStack ){
                captureStackHelper(engine, vl);
            }

        } else if ( info.Length() == 1 ){
            VisualLog vl(level);

            if ( captureStack ){
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

            v8::String::Utf8Value utfVal(engine->isolate(), arg->ToString(engine->isolate()->GetCurrentContext()).ToLocalChecked());
            vl << *utfVal;

        } else if ( info.Length() == 2 ){
            v8::String::Utf8Value utfKey(engine->isolate(), info[0]->ToString(engine->isolate()->GetCurrentContext()).ToLocalChecked());

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

            v8::String::Utf8Value utfVal(engine->isolate(), arg->ToString(engine->isolate()->GetCurrentContext()).ToLocalChecked());
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


    static v8::Local<v8::FunctionTemplate> functionTemplate(v8::Isolate* isolate){
        v8::Local<v8::FunctionTemplate> localTpl;
        localTpl = v8::FunctionTemplate::New(isolate);
        localTpl->SetClassName(v8::String::NewFromUtf8(isolate, "VisualLog").ToLocalChecked());

        v8::Local<v8::Template> tplPrototype = localTpl->PrototypeTemplate();

        tplPrototype->Set(isolate, "f", v8::FunctionTemplate::New(isolate, &VisualLogJsObject::f));
        tplPrototype->Set(isolate, "e", v8::FunctionTemplate::New(isolate, &VisualLogJsObject::e));
        tplPrototype->Set(isolate, "w", v8::FunctionTemplate::New(isolate, &VisualLogJsObject::w));
        tplPrototype->Set(isolate, "i", v8::FunctionTemplate::New(isolate, &VisualLogJsObject::i));
        tplPrototype->Set(isolate, "d", v8::FunctionTemplate::New(isolate, &VisualLogJsObject::d));
        tplPrototype->Set(isolate, "v", v8::FunctionTemplate::New(isolate, &VisualLogJsObject::v));
        tplPrototype->Set(isolate, "configure", v8::FunctionTemplate::New(isolate, &VisualLogJsObject::configure));

        return localTpl;
    }

public:
    VisualLogJsObject();
};

}} // namespace lv, el

#endif // VISUALLOGJSOBJECT_H
