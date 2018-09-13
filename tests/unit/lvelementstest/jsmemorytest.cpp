#include "jsmemorytest.h"
#include "live/elements/engine.h"
#include "live/elements/element.h"
#include "live/elements/module.h"

Q_TEST_RUNNER_REGISTER(JsMemoryTest);

using namespace lv;
using namespace lv::el;

class MemoryStub : public Element{

    META_OBJECT{
        META_OBJECT_DESCRIBE(MemoryStub)
            .base<Element>()
            .constructor<>()
        META_OBJECT_CLOSE
    }

public:
    MemoryStub(Engine* engine) : Element(engine){ refs()++; }
    ~MemoryStub(){ refs()--; }

    int elemenetRefs(){ return m_refs; }

public:
    static int& refs(){ static int refs; return refs; }

};

JsMemoryTest::JsMemoryTest(QObject *parent)
    : QObject(parent)
{
}

void JsMemoryTest::initTestCase(){
}

void JsMemoryTest::singleParentTest(){
    Engine* engine = new Engine;

    MemoryStub* p = new MemoryStub(engine);
    MemoryStub* e = new MemoryStub(engine);
    e->setParent(p);

    QCOMPARE(MemoryStub::refs(), 2);

    QVERIFY(e->elemenetRefs() == 1);
    QVERIFY(e->parent() == p);
    QVERIFY(p->totalChildren() == 1);
    e->setParent(nullptr);
    QVERIFY(e->parent() == nullptr);
    QVERIFY(p->totalChildren() == 0);
    QVERIFY(e->elemenetRefs() == 0);
    e->setParent(p);

    delete p;

    QVERIFY(MemoryStub::refs() == 0);

    delete engine;
}

void JsMemoryTest::multiParentTest(){
    Engine* engine = new Engine;
    MemoryStub* p = new MemoryStub(engine);
    MemoryStub* e1 = new MemoryStub(engine);
    MemoryStub* e2 = new MemoryStub(engine);

    QCOMPARE(MemoryStub::refs(), 3);
    e1->setParent(p);
    e2->setParent(e1);
    e2->setParent(p);
    QVERIFY(p->totalChildren() == 2);
    QVERIFY(e1->totalChildren() == 0);
    delete e1;
    QVERIFY(p->totalChildren() == 1);
    delete p;
    QCOMPARE(MemoryStub::refs(), 0);

    delete engine;
}

void JsMemoryTest::multiLevelTest(){
    Engine* engine = new Engine;
    MemoryStub* p  = new MemoryStub(engine);
    MemoryStub* e1 = new MemoryStub(engine);
    MemoryStub* m1 = new MemoryStub(engine);

    QCOMPARE(MemoryStub::refs(), 3);

    e1->setParent(p);
    m1->setParent(e1);
    QVERIFY(e1->totalChildren() == 1);
    QVERIFY(p->totalChildren() == 1);

    delete p;

    QCOMPARE(MemoryStub::refs(), 0);
    delete engine;
}

void JsMemoryTest::jsAllocationTest(){
    Engine* engine = new Engine();
    {
        MemoryStub* e = new MemoryStub(engine);
        engine->scope([&engine, &e](){
            LocalValue v(engine, e);

            QCOMPARE(MemoryStub::refs(), 1);
            MemoryStub* p = new MemoryStub(engine);
            e->setParent(p);

            QCOMPARE(e->elemenetRefs(), 1);

            delete p;
            QCOMPARE(MemoryStub::refs(), 1);
        });
        delete e;
    }

    delete engine;
}
