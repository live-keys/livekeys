#include "script.h"
#include "context_p.h"
#include "live/elements/engine.h"
#include "v8.h"

namespace lv{ namespace el{

class ScriptPrivate{
public:
    ScriptPrivate(Engine* e, const v8::Local<v8::Script>& d)
        : engine(e)
    {
        data.Reset(engine->isolate(), d);
    }

    Engine*                    engine;
    v8::Persistent<v8::Script> data;
};


const std::string Script::encloseStart = "(function(){";
const std::string Script::encloseEnd   = "})()";


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

Script::~Script(){
    m_d->data.Reset();
    delete m_d;
}

Script::Script(Engine *engine, const v8::Local<v8::Script> &value)
    : m_d(new ScriptPrivate(engine, value))
{
}

}} // namespace lv, script
