#include "imports_p.h"
#include "element_p.h"
#include "modulefile.h"
#include "live/elements/value.h"
#include "context_p.h"

namespace lv{ namespace el{

Imports::Imports(Engine *engine, ModuleFile *mf)
    : m_engine(engine)
    , m_moduleFile(mf)
{
    v8::Local<v8::Object> obj =
            engine->importsTemplate()->InstanceTemplate()->NewInstance(engine->currentContext()->asLocal()).ToLocalChecked();
    obj->SetInternalField(0, v8::External::New(engine->isolate(), this));

    m_object.Reset(engine->isolate(), obj);
}

Imports::~Imports(){
    m_object.Reset();
}

v8::Local<v8::Object> Imports::object(){
    return m_object.Get(m_engine->isolate());
}

void Imports::require(const std::string &importKey){
    ElementsModule::Ptr ep = m_engine->require(importKey, m_moduleFile->plugin()->plugin());
    for ( auto it = ep->fileExports().begin(); it != ep->fileExports().end(); ++it ){
        m_exports[it->first] = it->second;
    }

    const std::list<ModuleLibrary*>& libraries = ep->libraryModules();
    if ( libraries.size() > 0 ){
        m_libraryExports.push_back(ep->libraryExports());
    }
}

v8::Local<v8::Object> Imports::requireAs(const std::string &importKey){
    Imports* imp = new Imports(m_engine, m_moduleFile);
    imp->require(importKey);

    v8::Local<v8::Object> obj = imp->object();

    imp->m_object.SetWeak(imp, &Imports::weakImportsDestructor, v8::WeakCallbackType::kParameter);

    return obj;
}

v8::Local<v8::Value> Imports::get(const std::string &key){
    ModuleFile* mf = nullptr;

    ElementsModule::Ptr currentPlugin = m_moduleFile->plugin();
    auto currentPluginExportSearch = currentPlugin->fileExports().find(key);
    if ( currentPluginExportSearch != currentPlugin->fileExports().end() ){
        mf = currentPluginExportSearch->second;
    }

    v8::Local<v8::String> v8key = v8::String::NewFromUtf8(m_engine->isolate(), key.c_str()).ToLocalChecked();

    for ( auto it = m_libraryExports.begin(); it != m_libraryExports.end(); ++it ){
        v8::Local<v8::Object> vo = it->data();
        auto context = m_engine->currentContext()->asLocal();
        if ( vo->Has(context, v8key).ToChecked() ){
            return vo->Get(context, v8key).ToLocalChecked();
        }
    }

    auto exportSearch = m_exports.find(key);
    if ( exportSearch != m_exports.end() ){
        mf = exportSearch->second;
    }

    if ( !mf ){
        THROW_EXCEPTION(Exception, "Failed to find import at key: " + key, Exception::toCode("~Import"));
    }

    ScopedValue vl = mf->get(m_engine, m_moduleFile, key);
    v8::Local<v8::Value> value = vl.data();
    return value;
}

void Imports::get(const v8::FunctionCallbackInfo<v8::Value> &info){
    v8::Local<v8::Object> self = info.This();
    v8::Local<v8::External> wrap = v8::Local<v8::External>::Cast(self->GetInternalField(0));
    Imports* imports = reinterpret_cast<Imports*>(wrap->Value());
    Engine* engine = reinterpret_cast<Engine*>(info.GetIsolate()->GetData(0));

    if ( info.Length() != 1 ){
        info.GetIsolate()->ThrowException(v8::String::NewFromUtf8(
            info.GetIsolate(), "\'imports.get()\' needs one argument.").ToLocalChecked()
        );
        return;
    }

    try{
        v8::Local<v8::String> arg = info[0]->ToString(info.GetIsolate()->GetCurrentContext()).ToLocalChecked();
        v8::Local<v8::Value> val = imports->get(*v8::String::Utf8Value(info.GetIsolate(), arg));
        info.GetReturnValue().Set(val);
    } catch ( lv::Exception& e ){
        engine->throwError(&e, nullptr);
    }
}

void Imports::require(const v8::FunctionCallbackInfo<v8::Value> &info){
    v8::Local<v8::Object> self = info.This();
    v8::Local<v8::External> wrap = v8::Local<v8::External>::Cast(self->GetInternalField(0));
    Imports* imports = reinterpret_cast<Imports*>(wrap->Value());
    Engine* engine = reinterpret_cast<Engine*>(info.GetIsolate()->GetData(0));
    if ( info.Length() != 1 ){
        info.GetIsolate()->ThrowException(v8::String::NewFromUtf8(
            info.GetIsolate(), "\'imports.require()\' needs one argument.").ToLocalChecked()
        );
        return;
    }
    try{
        v8::Local<v8::String> arg = info[0]->ToString(info.GetIsolate()->GetCurrentContext()).ToLocalChecked();
        imports->require(*v8::String::Utf8Value(info.GetIsolate(), arg));
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
        info.GetIsolate()->ThrowException(v8::String::NewFromUtf8(info.GetIsolate(), "\'imports.requireAs()\' needs one argument.").ToLocalChecked());
        return;
    }
    try{
        v8::Local<v8::String> arg = info[0]->ToString(info.GetIsolate()->GetCurrentContext()).ToLocalChecked();
        v8::Local<v8::Object> obj = imports->requireAs(*v8::String::Utf8Value(info.GetIsolate(), arg));
        info.GetReturnValue().Set(obj);
    } catch ( lv::Exception& e ){
        engine->throwError(&e, nullptr);
    }
}

void Imports::getImport(v8::Local<v8::Name> name, const v8::PropertyCallbackInfo<v8::Value> &info){
    v8::Local<v8::Object> self = info.This();
    v8::Local<v8::External> wrap = v8::Local<v8::External>::Cast(self->GetInternalField(0));
    Imports* imports = reinterpret_cast<Imports*>(wrap->Value());

    if ( !name->IsString() ){
        info.GetIsolate()->ThrowException(v8::String::NewFromUtf8(
            info.GetIsolate(), "imports object accessor must be a string.").ToLocalChecked()
        );
        return;
    }

    v8::Local<v8::String> nameString = name->ToString(info.GetIsolate()->GetCurrentContext()).ToLocalChecked();
    std::string n = *v8::String::Utf8Value(info.GetIsolate(), nameString);

    if ( n == "require" || n == "requireAs" || n == "get" ){
        return;
    }

    try{
        v8::Local<v8::Value> val = imports->get(n);
        info.GetReturnValue().Set(val);
    } catch ( lv::Exception& ){
    }
}

v8::Local<v8::FunctionTemplate> Imports::functionTemplate(v8::Isolate *isolate){
    v8::Local<v8::FunctionTemplate> localTpl;
    localTpl = v8::FunctionTemplate::New(isolate);
    localTpl->SetClassName(v8::String::NewFromUtf8(isolate, "Imports").ToLocalChecked());

    v8::Local<v8::Template> tplPrototype = localTpl->PrototypeTemplate();

    tplPrototype->Set(isolate, "get", v8::FunctionTemplate::New(isolate, &Imports::get));
    tplPrototype->Set(isolate, "require", v8::FunctionTemplate::New(isolate, &Imports::require));
    tplPrototype->Set(isolate, "requireAs", v8::FunctionTemplate::New(isolate, &Imports::requireAs));

    v8::Local<v8::ObjectTemplate> tplInstance = localTpl->InstanceTemplate();
    tplInstance->SetInternalFieldCount(1);
    tplInstance->SetHandler(v8::NamedPropertyHandlerConfiguration(&Imports::getImport));

    return localTpl;
}

void Imports::weakImportsDestructor(const v8::WeakCallbackInfo<Imports> &data){
    Imports* i = data.GetParameter();
    i->m_object.Reset();
    delete i;
}

}} // namespce lv, el
