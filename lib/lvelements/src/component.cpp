#include "component.h"

namespace lv{ namespace el{


Component::Component(Engine *engine, const Callable &c)
    : m_engine(engine)
    , m_constructor(c)
{
}

Element *Component::create(const Function::Parameters &params){
    return m_constructor.callAsConstructor(m_engine, params).toElement(m_engine);
}

}} // namespace lv, el
