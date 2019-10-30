#include "function.h"
#include "live/elements/engine.h"
#include "live/elements/element.h"
#include "live/exception.h"
#include "v8nowarnings.h"

namespace lv{ namespace el{


template<>
bool Function::CallInfo::extractValue(const v8::FunctionCallbackInfo<v8::Value> *info, int index){
    return (*info)[index]->BooleanValue();
}

template<>
Value::Int32 Function::CallInfo::extractValue(v8::FunctionCallbackInfo<v8::Value> const* info, int index){
    return (*info)[index]->Int32Value();
}

template<>
Value::Int64 Function::CallInfo::extractValue(const v8::FunctionCallbackInfo<v8::Value> *info, int index){
    return (*info)[index]->IntegerValue();
}

template<>
Value::Number Function::CallInfo::extractValue(const v8::FunctionCallbackInfo<v8::Value> *info, int index){
    return (*info)[index]->NumberValue();
}

template<>
std::string Function::CallInfo::extractValue(const v8::FunctionCallbackInfo<v8::Value> *info, int index){
    return *v8::String::Utf8Value((*info)[index]->ToString(info->GetIsolate()));
}

template<>
Object Function::CallInfo::extractValue(const v8::FunctionCallbackInfo<v8::Value> *info, int index){
    Engine* engine = reinterpret_cast<Engine*>(info->GetIsolate()->GetData(0));
    return Object(engine, v8::Local<v8::Object>::Cast((*info)[index]));
}

template<>
Buffer Function::CallInfo::extractValue(const v8::FunctionCallbackInfo<v8::Value> *info, int index){
    return Buffer(v8::Local<v8::ArrayBuffer>::Cast((*info)[index]));
}

template<>
Value Function::CallInfo::extractValue(const v8::FunctionCallbackInfo<v8::Value> *info, int index){
    Engine* engine = reinterpret_cast<Engine*>(info->GetIsolate()->GetData(0));
    return LocalValue(engine, (*info)[index]).toValue(engine);
}

template<>
Callable Function::CallInfo::extractValue(const v8::FunctionCallbackInfo<v8::Value> *info, int index){
    v8::Local<v8::Function> fn = v8::Local<v8::Function>::Cast((*info)[index]);
    Engine* engine = reinterpret_cast<Engine*>(info->GetIsolate()->GetData(0));
    return Callable(engine, fn);
}

template<>
LocalValue Function::CallInfo::extractValue(const v8::FunctionCallbackInfo<v8::Value> *info, int index){
    return LocalValue((*info)[index]);
}

template<>
Element *Function::CallInfo::extractValue(const v8::FunctionCallbackInfo<v8::Value> *info, int index){
    v8::Local<v8::Object> vo = v8::Local<v8::Object>::Cast((*info)[index]);
    if ( vo->InternalFieldCount() != 1 ){
        Engine* engine = reinterpret_cast<Engine*>(info->GetIsolate()->GetData(0));
        lv::Exception exc = CREATE_EXCEPTION(lv::Exception, "Value cannot be converted to an Element type or subtype.", 1);
        engine->throwError(&exc, nullptr);
        return nullptr;
    }

    v8::Local<v8::External> wrap = v8::Local<v8::External>::Cast(vo->GetInternalField(0));
    void* ptr = wrap->Value();
    return reinterpret_cast<Element*>(ptr);
}


LocalValue Function::CallInfo::at(size_t index) const{
    return LocalValue((*m_info)[static_cast<int>(index)]);
}

size_t Function::CallInfo::length() const{
    return m_info->Length();
}

void *Function::CallInfo::userData(){
    return m_info->Data().As<v8::External>()->Value();
}

bool Function::CallInfo::clearedPendingException(Engine *engine) const{
    if ( engine->hasPendingException() ){
        engine->clearPendingException();
        return true;
    }
    return false;
}

void Function::CallInfo::throwError(Engine *engine, Exception *e) const{
    engine->throwError(e, reinterpret_cast<Element*>(internalField()));
}

void Function::CallInfo::throwError(Engine *engine, const std::string &message) const{
    lv::Exception e = CREATE_EXCEPTION(lv::Exception, message.c_str(), 1);
    throwError(engine, &e);
}

void Function::CallInfo::assignReturnValue(const v8::Local<v8::Value> &value) const{
    m_info->GetReturnValue().Set(value);
}

v8::Local<v8::Object> Function::CallInfo::thatObject(){
    return m_info->This();
}

Engine *Function::CallInfo::engine() const{
    return reinterpret_cast<Engine*>(m_info->GetIsolate()->GetData(0));
}

void *Function::CallInfo::internalField() const{
    v8::Local<v8::Object> self = m_info->This();
    v8::Local<v8::External> wrap = v8::Local<v8::External>::Cast(self->GetInternalField(0));
    return wrap->Value();
}

// Function::Paramters implementation
// -------------------------------------------------------------------------------

Function::Parameters::Parameters(int length)
    : m_length(length)
    , m_args(new v8::Local<v8::Value>[length])
{
}

Function::Parameters::Parameters(const std::initializer_list<LocalValue> &init)
    : m_length(static_cast<int>(init.size()))
    , m_args(new v8::Local<v8::Value>[static_cast<int>(init.size())])
{
    int index = 0;
    for ( auto it = init.begin(); it != init.end(); ++it ){
        m_args[index++] = it->data();
    }
}

Function::Parameters::~Parameters(){
    delete[] m_args;
}

void Function::Parameters::assign(int index, const LocalValue &ref){
    m_args[index] = ref.data();
}

void Function::Parameters::assign(int index, const v8::Local<v8::Value> &value){
    m_args[index] = value;
}

LocalValue Function::Parameters::at(Engine *engine, int index) const{
    return LocalValue(engine, m_args[index]);
}

int Function::Parameters::length() const{
    return m_length;
}


template<>
bool Function::Parameters::extractValue(Engine *, const v8::Local<v8::Value> *args, int index){
    return args[index]->BooleanValue();
}

template<>
int Function::Parameters::extractValue(Engine*, const v8::Local<v8::Value> *args, int index){
    return args[index]->Int32Value();
}

template<>
Value::Int64 Function::Parameters::extractValue(Engine*, const v8::Local<v8::Value> *args, int index){
    return args[index]->IntegerValue();
}

template<>
double Function::Parameters::extractValue(Engine*, const v8::Local<v8::Value> *args, int index){
    return args[index]->NumberValue();
}

template<>
std::string Function::Parameters::extractValue(Engine *engine, const v8::Local<v8::Value> *args, int index){
    return *v8::String::Utf8Value(args[index]->ToString(engine->isolate()));
}

template<>
Callable Function::Parameters::extractValue(Engine *engine, const v8::Local<v8::Value> *args, int index){
    return Callable(engine, v8::Local<v8::Function>::Cast(args[index]));
}

template<>
Object Function::Parameters::extractValue(Engine *engine, const v8::Local<v8::Value> *args, int index){
    return Object(engine, v8::Local<v8::Object>::Cast(args[index]));
}

template<>
Buffer Function::Parameters::extractValue(Engine *, const v8::Local<v8::Value> *args, int index){
    return Buffer(v8::Local<v8::ArrayBuffer>::Cast(args[index]));
}

template<>
LocalValue Function::Parameters::extractValue(Engine *engine, const v8::Local<v8::Value> *args, int index){
    return LocalValue(engine, args[index]);
}

template<>
Value Function::Parameters::extractValue(Engine *engine, const v8::Local<v8::Value> *args, int index){
    return LocalValue(engine, args[index]).toValue(engine);
}

template<>
Element *Function::Parameters::extractValue(Engine *engine, const v8::Local<v8::Value> *args, int index){
    if ( args[index]->IsNullOrUndefined() )
        return nullptr;

    v8::Local<v8::Object> vo = v8::Local<v8::Object>::Cast(args[index]);
    if ( vo->InternalFieldCount() != 1 ){
        lv::Exception e = CREATE_EXCEPTION(lv::Exception, "Value cannot be converted to an Element type or subtype.", 1);
        engine->throwError(&e, nullptr);
        return nullptr;
    }

    v8::Local<v8::External> wrap = v8::Local<v8::External>::Cast(vo->GetInternalField(0));
    void* ptr = wrap->Value();
    return reinterpret_cast<Element*>(ptr);
}



// Function implementation
// -------------------------------------------------------------------------------


void Function::assignReturnValue(const LocalValue &val, const v8::FunctionCallbackInfo<v8::Value> &info){
    info.GetReturnValue().Set(val.data());
}


}} // namespace lv, namespace script
