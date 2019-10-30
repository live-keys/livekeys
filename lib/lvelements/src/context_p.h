#ifndef LVCONTEXT_H
#define LVCONTEXT_H

#include "live/elements/lvelementsglobal.h"
#include "live/elements/engine.h"

#include "v8nowarnings.h"

namespace lv{ namespace el{

/**
 * @brief Wraps an engine context
 */
class Context{

public:
    Context(Engine* engine, const v8::Local<v8::Context>& context);
    ~Context();

    v8::Local<v8::Context> asLocal();
    Engine* engine() const;

private:
    Engine*                     m_engine;
    v8::Persistent<v8::Context> m_data;
};

inline v8::Local<v8::Context> Context::asLocal(){
    return m_data.Get(m_engine->isolate());
}

inline Engine *Context::engine() const{
    return m_engine;
}

}} // namespace lv, script

#endif // LVCONTEXT_H
