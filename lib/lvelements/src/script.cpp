#include "script.h"
#include "context_p.h"
#include "live/elements/engine.h"
#include "v8.h"
#include <QFileInfo>

namespace lv{ namespace el{

class ScriptPrivate{
public:
    ScriptPrivate(Engine* e, const v8::Local<v8::Script>& d)
        : engine(e)
    {
        data.Reset(engine->isolate(), d);
    }

    Engine*                    engine;
    std::string                path;
    v8::Persistent<v8::Script> data;
};


const std::string Script::encloseStart = "(function(){";
const std::string Script::encloseEnd   = "})()";

const std::string Script::moduleEncloseStart = "(function(module, imports, __NAME__, __FILE__){\n";
const std::string Script::moduleEncloseEnd   = "})";


Value Script::run(){
    v8::HandleScope handle(m_d->engine->isolate());
    v8::Local<v8::Context> context = m_d->engine->currentContext()->asLocal();
    v8::Context::Scope context_scope(context);

    v8::Local<v8::Script> ld = m_d->data.Get(m_d->engine->isolate());
    v8::MaybeLocal<v8::Value> result = ld->Run(context);
    if ( result.IsEmpty() )
        return LocalValue(m_d->engine).toValue(m_d->engine);
    else
        return LocalValue(result.ToLocalChecked()).toValue(m_d->engine);

    return Value();
}

Object Script::loadAsModule(){
    v8::HandleScope handle(m_d->engine->isolate());
    v8::Local<v8::Context> context = m_d->engine->currentContext()->asLocal();
    v8::Context::Scope context_scope(context);

    v8::Local<v8::Script> ld = m_d->data.Get(m_d->engine->isolate());
    v8::MaybeLocal<v8::Value> result = ld->Run(context);
    if ( result.IsEmpty() )
        throw std::exception();

    v8::Local<v8::Value> localResult = result.ToLocalChecked();
    v8::Local<v8::Function> loadFunction = v8::Local<v8::Function>::Cast(result.ToLocalChecked());

    LocalValue module = Module::createObject(m_d->engine, m_d->path);

    //TODO: Receive from package
    v8::Local<v8::Object> importsObject = v8::Object::New(m_d->engine->isolate());

    QFileInfo finfo(QString::fromStdString(m_d->path));

    v8::Local<v8::String> name = v8::String::NewFromUtf8(
        m_d->engine->isolate(), finfo.baseName().toStdString().c_str(), v8::String::kInternalizedString);
    v8::Local<v8::String> file = v8::String::NewFromUtf8(
        m_d->engine->isolate(), finfo.filePath().toStdString().c_str(), v8::String::kInternalizedString);

    v8::Local<v8::Value> args[4];
    args[0] = module.data();
    args[1] = importsObject;
    args[2] = name;
    args[3] = file;

    loadFunction->Call(context->Global(), 4, args);

    return module.toObject(m_d->engine);
}

Script::~Script(){
    m_d->data.Reset();
    delete m_d;
}

Script::Script(Engine *engine, const v8::Local<v8::Script> &value, const std::string &path)
    : m_d(new ScriptPrivate(engine, value))
{
    m_d->path = path;
}

}} // namespace lv, script
