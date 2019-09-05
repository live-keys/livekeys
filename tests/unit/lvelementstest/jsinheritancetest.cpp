#include "jsinheritancetest.h"

#include "live/elements/element.h"
#include "live/elements/metaobject.h"
#include "live/elements/component.h"
#include "live/elements/modulelibrary.h"

Q_TEST_RUNNER_REGISTER(JsInheritanceTest);

using namespace lv;
using namespace lv::el;

JsInheritanceTest::JsInheritanceTest(QObject *parent)
    : QObject(parent)
{

}

void JsInheritanceTest::initTestCase(){

}

void JsInheritanceTest::directElementTest(){
    Engine* engine = new Engine();
    engine->scope([&engine](){
        Value v = engine->compileEnclosed("return Element;")->run();
        QVERIFY(v.type() == Value::Stored::Callable);
        QVERIFY(v.asCallable().isComponent());
    });
    delete engine;
}

void JsInheritanceTest::objectPosingAsElementTest(){
    Engine* engine = new Engine();
    engine->scope([&engine](){
        Value v = engine->compileEnclosed("return Object;")->run();
        QVERIFY(v.type() == Value::Stored::Callable);
        QVERIFY(!v.asCallable().isComponent());
    });
    delete engine;
}

void JsInheritanceTest::stringPosingAsElementTest(){
    Engine* engine = new Engine();
    engine->scope([&engine](){
        Value v = engine->compileEnclosed("return String;")->run();
        QVERIFY(v.type() == Value::Stored::Callable);
        QVERIFY(!v.asCallable().isComponent());
    });
    delete engine;
}

void JsInheritanceTest::directInheritanceTest(){
    Engine* engine = new Engine();
    engine->scope([&engine](){
        Value v = engine->compileEnclosed("class A extends Element{}; return A;")->run();
        QVERIFY(v.type() == Value::Stored::Callable);
        QVERIFY(v.asCallable().isComponent());
    });
    delete engine;
}

void JsInheritanceTest::indirectInheritanceTest(){
    Engine* engine = new Engine();
    engine->scope([&engine](){
        Value v = engine->compileEnclosed("class A extends Element{}; class B extends A{}; return B;")->run();
        QVERIFY(v.type() == Value::Stored::Callable);
        QVERIFY(v.asCallable().isComponent());
    });
    delete engine;
}

void JsInheritanceTest::threeLevelInheritanceTest(){
    Engine* engine = new Engine();
    engine->scope([&engine](){
        Value v = engine->compileEnclosed(
            "class A extends Element{}; class B extends A{}; class C extends B{}; return C;"
        )->run();
        QVERIFY(v.type() == Value::Stored::Callable);
        QVERIFY(v.asCallable().isComponent());
    });
    delete engine;
}

void JsInheritanceTest::indirectCppInheritanceTest(){
    Engine* engine = new Engine();
    engine->scope([&engine](){
        Value v = engine->compileEnclosed("class A extends Container{}; return A;")->run();
        QVERIFY(v.type() == Value::Stored::Callable);
        QVERIFY(v.asCallable().isComponent());
    });
    delete engine;
}
