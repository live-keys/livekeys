#include "mlnodetojs.h"
#include "element_p.h"

namespace lv{
namespace ml{

namespace{

void toJs(const MLNode &n, v8::Local<v8::Value>& v, el::Engine *engine){
    switch( n.type() ){
    case MLNode::Type::Object: {
        v8::Local<v8::Object> lo = v8::Object::New(engine->isolate());
        for ( auto it = n.begin(); it != n.end(); ++it ){
            v8::Local<v8::Value> result;
            toJs(it.value(), result, engine);
            lo->Set(
                engine->isolate()->GetCurrentContext(),
                v8::String::NewFromUtf8(engine->isolate(), it.key().c_str()).ToLocalChecked(),
                result
            ).IsNothing();
        }
        v = lo;
        break;
    }
    case MLNode::Type::Array:{
        v8::Local<v8::Array> la = v8::Array::New(engine->isolate(), n.size());

        int index = 0;
        for ( auto it = n.begin(); it != n.end(); ++it ){
            v8::Local<v8::Value> result;
            toJs(it.value(), result, engine);
            la->Set(engine->isolate()->GetCurrentContext(), index, result).IsNothing();
            ++index;
        }
        v = la;
        break;
    }
    case MLNode::Type::Bytes:{
        v = v8::ArrayBuffer::New(engine->isolate(), n.asBytes().data(), n.asBytes().size());
        break;
    }
    case MLNode::Type::String:{
        v = v8::String::NewFromUtf8(engine->isolate(), n.asString().c_str()).ToLocalChecked();
        break;
    }
    case MLNode::Type::Boolean:{
        v = v8::Boolean::New(engine->isolate(), n.asBool());
        break;
    }
    case MLNode::Type::Integer:{
        v = v8::Integer::New(engine->isolate(), n.asInt());
        break;
    }
    case MLNode::Type::Float:{
        v = v8::Number::New(engine->isolate(), n.asFloat());
        break;
    }
    default:
        v = v8::Null(engine->isolate());
        break;
    }
}

void fromJs(const v8::Local<v8::Value> v, MLNode& n, el::Engine* engine){
    auto isolate = engine->isolate();
    auto context = isolate->GetCurrentContext();
    if ( v->IsArray() ){
        v8::Local<v8::Array> va = v8::Local<v8::Array>::Cast(v);
        n = MLNode(MLNode::Type::Array);
        for ( unsigned int i = 0; i < va->Length(); ++i ){
            MLNode result;
            fromJs(va->Get(context, i).ToLocalChecked(), result, engine);
            n.append(result);
        }
    } else if ( v->IsString() || v->IsStringObject() ){
        n = MLNode(*v8::String::Utf8Value(isolate, v->ToString(context).ToLocalChecked()));
    } else if ( v->IsObject() ){
        v8::Local<v8::Object> vo = v8::Local<v8::Object>::Cast(v);
        n = MLNode(MLNode::Type::Object);

        v8::Local<v8::Array> pn = vo->GetOwnPropertyNames(context, v8::ALL_PROPERTIES).ToLocalChecked();

        for (uint32_t i = 0; i < pn->Length(); ++i) {
            v8::Local<v8::Value> key = pn->Get(context, i).ToLocalChecked();
            v8::Local<v8::Value> value = vo->Get(context, key).ToLocalChecked();

            v8::String::Utf8Value utf8Key(isolate, key);
            MLNode result;
            fromJs(value, result, engine);


            n[*utf8Key] = result;
        }

    } else if ( v->IsBoolean() ){
        n = MLNode(v->BooleanValue(isolate));
    } else if ( v->IsInt32() ){
        n = MLNode(v->IntegerValue(context).ToChecked());
    } else if ( v->IsNumber() ){
        n = MLNode(v->NumberValue(context).ToChecked());
    } else {
        n = MLNode();
    }
}

}// namespace


void toJs(const MLNode &n, el::ScopedValue &v, el::Engine *engine){
    v8::Local<v8::Value> result;
    toJs(n, result, engine);
    v = el::ScopedValue(result);
}

void fromJs(const el::ScopedValue &v, MLNode &n, el::Engine* engine){
    fromJs(v.data(), n, engine);
}


} // namespace ml
} // namespace
