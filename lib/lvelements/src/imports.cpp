#include "imports_p.h"
#include "element_p.h"
#include "live/elements/value.h"

namespace lv{ namespace el{

Imports::Imports(Engine *engine, ModuleFile *mf)
    : m_engine(engine)
    , m_moduleFile(mf)
{
    v8::Local<v8::Object> obj = engine->importsTemplate()->InstanceTemplate()->NewInstance();
    m_object.Reset(engine->isolate(), obj);

    obj->SetInternalField(0, v8::External::New(engine->isolate(), this));
}

Imports::~Imports(){
    m_object.Reset();
}

v8::Local<v8::Object> Imports::object(){
    return m_object.Get(m_engine->isolate());
}

void Imports::require(const std::string &importKey){
    ElementsPlugin::Ptr ep = m_engine->require(importKey);
    //TODO store all ep exports, names to MF and names to ML, ML will always return the same object
}

v8::Local<v8::Object> Imports::requireAs(const std::string &importKey){
    ElementsPlugin::Ptr ep = m_engine->require(importKey);
    //TODO: Create new imports object
}

v8::Local<v8::Value> Imports::get(const std::string &key){
}

void Imports::require(const v8::FunctionCallbackInfo<v8::Value> &info){
    v8::Local<v8::Object> self = info.This();
    v8::Local<v8::External> wrap = v8::Local<v8::External>::Cast(self->GetInternalField(0));
    Imports* imports = reinterpret_cast<Imports*>(wrap->Value());
    Engine* engine = reinterpret_cast<Engine*>(info.GetIsolate()->GetData(0));
    if ( info.Length() != 1 ){
        info.GetIsolate()->ThrowException(v8::String::NewFromUtf8(
            info.GetIsolate(), "\'imports.require()\' needs one argument.")
        );
        return;
    }
    try{
        v8::Local<v8::String> arg = info[0]->ToString(info.GetIsolate());
        imports->require(*v8::String::Utf8Value(arg));
    } catch ( lv::Exception& e ){
        engine->throwError(&e, nullptr);
    }
}

void Imports::requireAs(const v8::FunctionCallbackInfo<v8::Value> &info){
    v8::Local<v8::Object> self = info.This();
    v8::Local<v8::External> wrap = v8::Local<v8::External>::Cast(self->GetInternalField(0));
    Imports* imports = reinterpret_cast<Imports*>(wrap->Value());
    Engine* engine = reinterpret_cast<Engine*>(info.GetIsolate()->GetData(0));
    if ( info.Length() != 1 ){
        info.GetIsolate()->ThrowException(v8::String::NewFromUtf8(info.GetIsolate(), "\'imports.require()\' needs one argument."));
        return;
    }
    try{
        v8::Local<v8::String> arg = info[0]->ToString(info.GetIsolate());
        v8::Local<v8::Object> obj = imports->requireAs(*v8::String::Utf8Value(arg));
        info.GetReturnValue().Set(obj);
    } catch ( lv::Exception& e ){
        engine->throwError(&e, nullptr);
    }
}

void Imports::getImport(const v8::Local<v8::String> &property, const v8::PropertyCallbackInfo<v8::Value> info){

}

v8::Local<v8::FunctionTemplate> Imports::functionTemplate(v8::Isolate *isolate){
    v8::Local<v8::FunctionTemplate> localTpl;
    localTpl = v8::FunctionTemplate::New(isolate);
    localTpl->SetClassName(v8::String::NewFromUtf8(isolate, "Imports"));

    v8::Local<v8::Template> tplPrototype = localTpl->PrototypeTemplate();

    tplPrototype->Set(v8::String::NewFromUtf8(isolate, "require"), v8::FunctionTemplate::New(isolate, &Imports::require));
    tplPrototype->Set(v8::String::NewFromUtf8(isolate, "requireAs"), v8::FunctionTemplate::New(isolate, &Imports::requireAs));

    v8::Local<v8::ObjectTemplate> tplInstance = localTpl->InstanceTemplate();
    tplInstance->SetInternalFieldCount(1);
//    tplInstance->SetHandler(); //TODO

    return localTpl;
}

}} // namespce lv, el
