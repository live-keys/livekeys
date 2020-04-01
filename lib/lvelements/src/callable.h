#ifndef LVCALLABLE_H
#define LVCALLABLE_H

#include "live/elements/lvelementsglobal.h"
#include "live/elements/function.h"

namespace lv{ namespace el{

class CallablePrivate;

/**
 * @brief Stores a callable js function.
 *
 * The callable object will keep the reference to the js function alive till the object persists,
 * after which it will be collected by the garbage collector.
 */
class LV_ELEMENTS_EXPORT Callable{

    friend class ScopedValue;
    friend class Engine;

public:
    Callable(Engine* engine);
    Callable(const Callable& other);
    Callable(Engine *engine, const v8::Local<v8::Function>& value); // Construct from engine value
    ~Callable();

    Callable& operator = (const Callable& other);
    bool operator == (const Callable& other);

    bool isNull() const;
    bool isComponent() const;
    Component toComponent() const;

    ScopedValue call(Element *that, const Function::Parameters& params) const;
    ScopedValue call(Engine* engine, const Function::Parameters& params) const;
    ScopedValue callAsConstructor(Engine* engine, const Function::Parameters& params) const;

private:
    v8::Local<v8::Function> data() const;

    CallablePrivate* m_d;
    int*             m_ref;
};



}} // namespace lv, el


#endif // LVCALLABLE_H
