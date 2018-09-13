#ifndef LVCOMPONENT_H
#define LVCOMPONENT_H

#include "live/elements/lvelementsglobal.h"
#include "live/elements/value.h"
#include "live/elements/callable.h"

namespace lv{ namespace el{

/**
 * @brief Wraps a template for creating new elements
 */
class LV_ELEMENTS_EXPORT Component{

    friend class Callable;

public:
    Element* create(const Function::Parameters& params);

private:
    Component(Engine* engine, const Callable& c);

    Engine*  m_engine;
    Callable m_constructor;
};

}} // namespace lv, script

#endif // LVCOMPONENT_H
