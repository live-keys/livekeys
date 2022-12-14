#ifndef LVTESTER_H
#define LVTESTER_H

#include "live/elements/engine.h"
#include "live/elements/element.h"

namespace lv{ namespace el{

class Tester : public Element{

    META_OBJECT{
        META_OBJECT_DESCRIBE(Tester)
            .base<Element>()
            .constructor()
            .scriptMethod("assert",       &Tester::assertImpl)
            .scriptMethod("assertThrows", &Tester::assertThrowsImpl)
        META_OBJECT_CLOSE
    }

public:
    Tester(Engine* engine);
    ~Tester() override;

    void assertImpl(const Function::CallInfo& ci);
    void assertThrowsImpl(const Function::CallInfo& ci);
};

}} // namespace lv, el

#endif // LVTESTER_H
