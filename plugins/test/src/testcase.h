#ifndef LVTESTASE_H
#define LVTESTASE_H

#include "live/elements/element.h"
#include "live/elements/list.h"
#include "scenario.h"

namespace lv{ namespace el{

class TestCase : public Element{

    META_OBJECT{
        META_OBJECT_DESCRIBE(TestCase)
            .constructor()
            .base<Element>()
            .scriptProperty<LocalValue>("children", &TestCase::children, &TestCase::setChildren, "childrenChanged")
            .scriptProperty<Callable>("beforeEach", &TestCase::beforeEach, &TestCase::setBeforeEach, "beforeEachChanged")
            .scriptProperty<Callable>("afterEach", &TestCase::afterEach, &TestCase::setAfterEach, "afterEachChanged")
            .scriptProperty<Callable>("beforeAll", &TestCase::beforeAll, &TestCase::setBeforeAll, "beforeAllChanged")
            .scriptProperty<Callable>("afterAll", &TestCase::afterAll, &TestCase::setAfterAll, "afterAllChanged")
            .scriptDefaultProperty("children")
            .scriptEvent("childrenChanged", &TestCase::childrenChanged)
        META_OBJECT_CLOSE
    }

public:
    TestCase(Engine* engine);
    ~TestCase() override;

    LocalValue children();
    void setChildren(LocalValue children);

    const std::vector<Element*> data() const;

    void setBeforeEach(Callable c);
    void setBeforeAll(Callable c);
    void setAfterEach(Callable c);
    void setAfterAll(Callable c);

    const Callable& beforeEach() const;
    const Callable& beforeAll() const;
    const Callable& afterEach() const;
    const Callable& afterAll() const;

    void onReady() override;

public:
    Event childrenChanged();
    Event beforeEachChanged();
    Event afterEachChanged();
    Event beforeAllChanged();
    Event afterAllChanged();

private:
    std::vector<Element*> m_data;

    Callable m_beforeEach;
    Callable m_beforeAll;
    Callable m_afterEach;
    Callable m_afterAll;

    bool m_ready;
};

inline void TestCase::setBeforeEach(Callable c){
    m_beforeEach = c;
    beforeEachChanged();
}

inline void TestCase::setBeforeAll(Callable c){
    m_beforeAll = c;
    beforeAllChanged();
}

inline void TestCase::setAfterEach(Callable c){
    m_afterEach = c;
    afterEachChanged();
}

inline void TestCase::setAfterAll(Callable c){
    m_afterAll = c;
    afterAllChanged();
}

inline const Callable &TestCase::beforeEach() const{
    return m_beforeEach;
}

inline const Callable &TestCase::beforeAll() const{
    return m_beforeAll;
}

inline const Callable &TestCase::afterEach() const{
    return m_afterEach;
}

inline const Callable &TestCase::afterAll() const{
    return m_afterAll;
}

inline Event TestCase::childrenChanged(){
    static Event::Id eid = eventId(&TestCase::childrenChanged);
    return notify(eid);
}

inline Event TestCase::beforeEachChanged(){
    static Event::Id eid = eventId(&TestCase::beforeEachChanged);
    return notify(eid);
}

inline Event TestCase::afterEachChanged(){
    static Event::Id eid = eventId(&TestCase::afterEachChanged);
    return notify(eid);
}

inline Event TestCase::beforeAllChanged(){
    static Event::Id eid = eventId(&TestCase::beforeAllChanged);
    return notify(eid);
}

inline Event TestCase::afterAllChanged(){
    static Event::Id eid = eventId(&TestCase::afterAllChanged);
    return notify(eid);
}

}} // namespace lv, el

#endif // LVTESTASE_H
