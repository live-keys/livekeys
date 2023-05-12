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

#include "jsmodule.h"
#include "v8nowarnings.h"
#include "live/elements/engine.h"
#include "live/path.h"

namespace lv{ namespace el{

class JsModulePrivate{
public:
    v8::Persistent<v8::Module> module;
};

JsModule::Ptr JsModule::create(Engine* engine, const std::string &file, const v8::Local<v8::Module> &mod){
    JsModule* jm = new JsModule(engine, file, mod);

    JsModule::Ptr jmptr = JsModule::Ptr(jm);
    return jmptr;
}

v8::Local<v8::Module> JsModule::localModule() const{
    return d_ptr->module.Get(m_engine->isolate());
}

v8::Local<v8::Value> JsModule::localModuleNamespace(){
    return localModule()->GetModuleNamespace();
}

v8::Local<v8::Value> JsModule::evaluate(v8::Local<v8::Context> ctx){
    v8::Local<v8::Value> retValue;

    v8::Local<v8::Module> mod = localModule();

    if (!mod->Evaluate(ctx).ToLocal(&retValue)) {
        THROW_EXCEPTION(lv::Exception, "Failed to evaluate module: " + m_file, lv::Exception::toCode("~Module"));
    }
    if ( mod->GetStatus() == v8::Module::kErrored ){
        v8::Local<v8::Object> excp = mod->GetException()->ToObject(ctx).ToLocalChecked();
        v8::Local<v8::String> messageKey = v8::String::NewFromUtf8(m_engine->isolate(), "message", v8::NewStringType::kInternalized).ToLocalChecked();
        v8::Local<v8::String> fileNameKey = v8::String::NewFromUtf8(m_engine->isolate(), "fileName", v8::NewStringType::kInternalized).ToLocalChecked();
        v8::Local<v8::String> lineNumberKey = v8::String::NewFromUtf8(m_engine->isolate(), "lineNumber", v8::NewStringType::kInternalized).ToLocalChecked();

        int lineNumber = static_cast<int>(excp->Get(ctx, lineNumberKey).ToLocalChecked()->ToInteger(ctx).ToLocalChecked()->Value());
        std::string fileName = "", message = "";
        if ( excp->Has(ctx, fileNameKey).ToChecked() ){
            v8::String::Utf8Value result(m_engine->isolate(), excp->Get(ctx, fileNameKey).ToLocalChecked());
            fileName = *result;
        } else {
            fileName = m_file;
        }
        if ( excp->Has(ctx, messageKey).ToChecked() ){
            v8::String::Utf8Value result(m_engine->isolate(), excp->Get(ctx, messageKey).ToLocalChecked());
            message = *result;
        }

        std::string lineNumberStr = lineNumber > 0 ? ":" + std::to_string(lineNumber) : "";
        THROW_EXCEPTION(Exception, "Uncaught exception: " + message + " at " + fileName + lineNumberStr, Exception::toCode("~Module"));
    }

    return retValue;
}

ScopedValue JsModule::moduleNamespace(){
    return ScopedValue(localModule()->GetModuleNamespace());
}

ScopedValue JsModule::evaluate(){
    auto isolate = m_engine->isolate();
    auto context = isolate->GetCurrentContext();

    return ScopedValue(evaluate(context));
}

const std::string &JsModule::name() const{
    return m_name;
}

JsModule::Status JsModule::status() const{
    v8::Local<v8::Module> mod = localModule();
    if ( mod->GetStatus() == v8::Module::kEvaluated )
        return JsModule::Ready;
    else if ( mod->GetStatus() == v8::Module::kEvaluating )
        return JsModule::Evaluating;
    return JsModule::Initialized;
}

JsModule::JsModule(Engine* engine, const std::string& file, const v8::Local<v8::Module>& mod)
    : d_ptr(new JsModulePrivate)
    , m_file(file)
    , m_moduleFile(nullptr)
    , m_engine(engine)
{
    m_name = Path::baseName(file);
    d_ptr->module.Reset(engine->isolate(), mod);
}

JsModule::~JsModule(){
    d_ptr->module.Reset();
    delete d_ptr;
}

}}// namespace lv, el
