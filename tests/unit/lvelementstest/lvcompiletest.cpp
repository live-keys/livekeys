#include "lvcompiletest.h"
#include "live/elements/engine.h"
#include "live/elements/element.h"
#include "live/elements/metaobject.h"
#include "live/elements/modulelibrary.h"
#include "live/elements/component.h"
#include "live/elements/container.h"
#include "live/applicationcontext.h"

Q_TEST_RUNNER_REGISTER(LvCompileTest);

using namespace lv;
using namespace lv::el;

LvCompileTest::LvCompileTest(QObject *parent)
    : QObject(parent)
{
}

void LvCompileTest::initTestCase(){
}

void LvCompileTest::test1Lv(){
    lv::el::Engine* engine = new lv::el::Engine(nullptr, Compiler::create(Compiler::Config(true, ".test.js")));
    engine->setModuleFileType(Engine::Lv);
    engine->scope([engine](){
        std::string scriptsPath = lv::ApplicationContext::instance().releasePath() + "/data/Test1.lv";
        ElementsModule::Ptr epl = engine->compile(scriptsPath);

        ModuleFile* mf = epl->moduleFileBypath(scriptsPath);
        JsModule::Ptr jsMod = engine->loadAsJsModule(mf);
        jsMod->evaluate();

        ScopedValue sv = jsMod->moduleNamespace();
        Object::Accessor oa(sv);

        ScopedValue rootValue = oa.get(engine, "Test1");
        Callable c = rootValue.toCallable(engine);
        QVERIFY(c.isComponent());

        Component test1 = c.toComponent();
        Element* elem = test1.create(Function::Parameters(0));

        QVERIFY(elem->hasProperty("y"));
        QVERIFY(elem->get("y").toInt32(engine) == 20);

        QVERIFY(elem->hasProperty("t"));
        QVERIFY(elem->get("t").toInt32(engine) == 30);

        QVERIFY(elem->hasProperty("x"));
        QVERIFY(elem->get("x").toInt32(engine) == 50);

        elem->set("y", ScopedValue(engine, 100));
        QVERIFY(elem->get("x").toInt32(engine) == 130);

        elem->set("t", ScopedValue(engine, 200));
        QVERIFY(elem->get("x").toInt32(engine) == 300);

        elem->set("x", ScopedValue(engine, 100));
        QVERIFY(elem->get("x").toInt32(engine) == 100);

        elem->set("y", ScopedValue(engine, 1000));
        QVERIFY(elem->get("x").toInt32(engine) == 100);
    });

    delete engine;
}

void LvCompileTest::test2Lv(){
    lv::el::Engine* engine = new lv::el::Engine(nullptr, Compiler::create(Compiler::Config(true, ".test.js")));
    engine->scope([engine](){
        std::string scriptsPath = lv::ApplicationContext::instance().releasePath() + "/data/Test2.lv";
        ElementsModule::Ptr epl = engine->compile(scriptsPath);

        ModuleFile* mf = epl->moduleFileBypath(scriptsPath);
        JsModule::Ptr jsMod = engine->loadAsJsModule(mf);
        jsMod->evaluate();

        ScopedValue sv = jsMod->moduleNamespace();
        Object::Accessor oa(sv);

        ScopedValue rootValue = oa.get(engine, "Test2");
        Callable c = rootValue.toCallable(engine);
        QVERIFY(c.isComponent());

        Component test2 = c.toComponent();
        Element* elem = test2.create(Function::Parameters(0));

        QVERIFY(elem->hasProperty("x"));
        QVERIFY(elem->get("x").toInt32(engine) == 20);

        Container* container = elem->cast<Container>();
        QVERIFY(container);
        QVERIFY(container->data().size() == 1);

        Element* child0 = container->data().at(0);
        QVERIFY(child0->hasProperty("l"));
        QVERIFY(child0->get("l").toInt32(engine) == 40);

        elem->set("x", ScopedValue(engine, 100));
        QVERIFY(child0->get("l").toInt32(engine) == 120);
    });


    delete engine;
}

void LvCompileTest::test3Lv(){
    lv::el::Engine* engine = new lv::el::Engine();
    engine->scope([engine](){
        std::string scriptsPath = lv::ApplicationContext::instance().releasePath() + "/data/Test3.lv";
        ElementsModule::Ptr epl = engine->compile(scriptsPath);

        ModuleFile* mf = epl->moduleFileBypath(scriptsPath);
        JsModule::Ptr jsMod = engine->loadAsJsModule(mf);
        jsMod->evaluate();

        ScopedValue sv = jsMod->moduleNamespace();
        Object::Accessor oa(sv);

        ScopedValue rootValue = oa.get(engine, "Test3");
        Callable c = rootValue.toCallable(engine);
        QVERIFY(c.isComponent());

        Component test3 = c.toComponent();
        Element* elem = test3.create(Function::Parameters(0));

        QVERIFY(elem->hasProperty("elemProp"));
        QVERIFY(elem->get("elemProp").isElement());

        QVERIFY(elem->get("elemProp").toElement(engine)->hasProperty("x"));
        QVERIFY(elem->get("elemProp").toElement(engine)->get("x").toInt32(engine) == 20);
        QVERIFY(elem->get("elemProp").toElement(engine)->getId() == "twenty");

        Container* container = elem->cast<Container>();
        QVERIFY(container);
        QVERIFY(container->data().size() == 1);

        Element* child0 = container->data().at(0);
        QVERIFY(child0->hasProperty("y"));
        QVERIFY(child0->get("y").toInt32(engine) == 20);

        elem->get("elemProp").toElement(engine)->set("x", ScopedValue(engine, 100));
        QVERIFY(child0->get("y").toInt32(engine) == 100);
    });


    delete engine;
}

void LvCompileTest::test4Lv(){
    lv::el::Engine* engine = new lv::el::Engine();
    engine->scope([engine](){
        std::string scriptsPath = lv::ApplicationContext::instance().releasePath() + "/data/Test4.lv";
        ElementsModule::Ptr epl = engine->compile(scriptsPath);

        ModuleFile* mf = epl->moduleFileBypath(scriptsPath);
        JsModule::Ptr jsMod = engine->loadAsJsModule(mf);
        jsMod->evaluate();

        ScopedValue sv = jsMod->moduleNamespace();
        Object::Accessor oa(sv);

        ScopedValue rootValue = oa.get(engine, "Test4");
        Callable c = rootValue.toCallable(engine);
        QVERIFY(c.isComponent());

        Component test4 = c.toComponent();
        Element* elem = test4.create(Function::Parameters(0));

        QVERIFY(elem->hasProperty("x"));
        QVERIFY(elem->get("x").toInt32(engine) == 20);
        QVERIFY(elem->hasProperty("y"));
        QVERIFY(elem->get("y").toInt32(engine) == 20);

        elem->trigger("dataChanged", Function::Parameters(0));
        QVERIFY(elem->get("x").toInt32(engine) == 100);

        Container* container = elem->cast<Container>();
        QVERIFY(container);
        QVERIFY(container->data().size() == 2);

        Element* child0 = container->data().at(0);
        QVERIFY(child0->hasProperty("k"));
        QVERIFY(child0->get("k").toInt32(engine) == 20);

        Element* child1 = container->data().at(1);
        QVERIFY(child1->hasProperty("message"));
        QVERIFY(child1->get("message").toStdString(engine) == "thirty");
    });


    delete engine;
}

void LvCompileTest::test1Js(){
    lv::el::Engine* engine = new lv::el::Engine();
    engine->scope([engine](){
        std::string scriptsPath = lv::ApplicationContext::instance().releasePath() + "/data/Test1.lv.js";
        JsModule::Ptr jsMod = engine->loadJsModule(scriptsPath);
        jsMod->evaluate();

        ScopedValue sv = jsMod->moduleNamespace();
        Object::Accessor le(sv);

        Callable c = le.get(engine, "Test1").toCallable(engine);
        QVERIFY(c.isComponent());

        Component test1 = c.toComponent();
        Element* elem = test1.create(Function::Parameters(0));

        QVERIFY(elem->hasProperty("y"));
        QVERIFY(elem->get("y").toInt32(engine) == 20);

        QVERIFY(elem->hasProperty("t"));
        QVERIFY(elem->get("t").toInt32(engine) == 30);

        QVERIFY(elem->hasProperty("x"));
        QVERIFY(elem->get("x").toInt32(engine) == 50);

        elem->set("y", ScopedValue(engine, 100));
        QVERIFY(elem->get("x").toInt32(engine) == 130);

        elem->set("t", ScopedValue(engine, 200));
        QVERIFY(elem->get("x").toInt32(engine) == 300);

        elem->set("x", ScopedValue(engine, 100));
        QVERIFY(elem->get("x").toInt32(engine) == 100);

        elem->set("y", ScopedValue(engine, 1000));
        QVERIFY(elem->get("x").toInt32(engine) == 100);
    });

    delete engine;
}

void LvCompileTest::test2Js(){
    lv::el::Engine* engine = new lv::el::Engine();
    engine->scope([engine](){
        std::string scriptsPath = lv::ApplicationContext::instance().releasePath() + "/data/Test2.lv.js";
        JsModule::Ptr jsMod = engine->loadJsModule(scriptsPath);
        jsMod->evaluate();

        ScopedValue sv = jsMod->moduleNamespace();
        Object::Accessor le(sv);

        Callable c = le.get(engine, "Test2").toCallable(engine);
        QVERIFY(c.isComponent());

        Component test2 = c.toComponent();
        Element* elem = test2.create(Function::Parameters(0));

        QVERIFY(elem->hasProperty("x"));
        QVERIFY(elem->get("x").toInt32(engine) == 20);

        Container* container = elem->cast<Container>();
        QVERIFY(container);
        QVERIFY(container->data().size() == 1);

        Element* child0 = container->data().at(0);
        QVERIFY(child0->hasProperty("l"));
        QVERIFY(child0->get("l").toInt32(engine) == 40);

        elem->set("x", ScopedValue(engine, 100));
        QVERIFY(child0->get("l").toInt32(engine) == 120);
    });


    delete engine;
}

void LvCompileTest::test3Js(){
    lv::el::Engine* engine = new lv::el::Engine();
    engine->scope([engine](){
        std::string scriptsPath = lv::ApplicationContext::instance().releasePath() + "/data/Test3.lv.js";
        JsModule::Ptr jsMod = engine->loadJsModule(scriptsPath);
        jsMod->evaluate();

        ScopedValue sv = jsMod->moduleNamespace();
        Object::Accessor le(sv);

        Callable c = le.get(engine, "Test3").toCallable(engine);
        QVERIFY(c.isComponent());

        Component test3 = c.toComponent();
        Element* elem = test3.create(Function::Parameters(0));

        QVERIFY(elem->hasProperty("elemProp"));
        QVERIFY(elem->get("elemProp").isElement());

        QVERIFY(elem->get("elemProp").toElement(engine)->hasProperty("x"));
        QVERIFY(elem->get("elemProp").toElement(engine)->get("x").toInt32(engine) == 20);
        QVERIFY(elem->get("elemProp").toElement(engine)->getId() == "twenty");

        Container* container = elem->cast<Container>();
        QVERIFY(container);
        QVERIFY(container->data().size() == 1);

        Element* child0 = container->data().at(0);
        QVERIFY(child0->hasProperty("y"));
        QVERIFY(child0->get("y").toInt32(engine) == 20);

        elem->get("elemProp").toElement(engine)->set("x", ScopedValue(engine, 100));
        QVERIFY(child0->get("y").toInt32(engine) == 100);
    });


    delete engine;
}

void LvCompileTest::test4Js(){
    lv::el::Engine* engine = new lv::el::Engine();
    engine->scope([engine](){
        std::string scriptsPath = lv::ApplicationContext::instance().releasePath() + "/data/Test4.lv.js";
        JsModule::Ptr jsMod = engine->loadJsModule(scriptsPath);
        jsMod->evaluate();

        ScopedValue sv = jsMod->moduleNamespace();
        Object::Accessor le(sv);

        Callable c = le.get(engine, "Test4").toCallable(engine);
        QVERIFY(c.isComponent());

        Component test4 = c.toComponent();
        Element* elem = test4.create(Function::Parameters(0));

        QVERIFY(elem->hasProperty("x"));
        QVERIFY(elem->get("x").toInt32(engine) == 20);
        QVERIFY(elem->hasProperty("y"));
        QVERIFY(elem->get("y").toInt32(engine) == 20);

        elem->trigger("dataChanged", Function::Parameters(0));
        QVERIFY(elem->get("x").toInt32(engine) == 100);

        Container* container = elem->cast<Container>();
        QVERIFY(container);
        QVERIFY(container->data().size() == 2);

        Element* child0 = container->data().at(0);
        QVERIFY(child0->hasProperty("k"));
        QVERIFY(child0->get("k").toInt32(engine) == 20);

        Element* child1 = container->data().at(1);
        QVERIFY(child1->hasProperty("message"));
        QVERIFY(child1->get("message").toStdString(engine) == "thirty");
    });


    delete engine;
}
