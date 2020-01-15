#include "context_p.h"

namespace lv{ namespace el{


Context::Context(Engine *engine, const v8::Local<v8::Context> &context)
    : m_engine(engine)
{
    m_data.Reset(engine->isolate(), context);
}

Context::~Context(){
    m_data.Reset();
}

}}// namespace lv, el
