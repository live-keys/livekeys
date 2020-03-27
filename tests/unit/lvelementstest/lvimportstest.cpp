#include "lvimportstest.h"
#include "live/elements/engine.h"
#include "live/applicationcontext.h"
#include "live/exception.h"
#include "live/elements/component.h"
#include "live/elements/element.h"

#include "testpack.h"
Q_TEST_RUNNER_REGISTER(LvImportsTest);

using namespace lv;
using namespace lv::el;

LvImportsTest::LvImportsTest(QObject *parent) : QObject(parent)
{

}

void LvImportsTest::initTestCase()
{

}

void LvImportsTest::singlePluginImportTest(){
    Engine* engine = new Engine;
    engine->setModuleFileType(Engine::Lv);
    std::string scriptsPath = lv::ApplicationContext::instance().releasePath() + "/data/ImportTest01.lv";
    std::string testPath = (QCoreApplication::applicationDirPath() + "/test").toStdString();

    TestPack tp(testPath);
    tp.unpack(scriptsPath);

    engine->scope([engine, testPath](){
        Object exports = engine->loadFile(testPath + "/main.lv");
        Object::Accessor localExports(exports);

        ScopedValue lv = localExports.get(engine, "main");

        QVERIFY(lv.isCallable());
        Callable c = lv.toCallable(engine);
        QVERIFY(c.isComponent());

        Component comp = c.toComponent();
        Element* el = comp.create(Function::Parameters(0));

        QVERIFY(el != nullptr);
        QVERIFY(el->get("a").toStdString(engine) == "class[A]");
        QVERIFY(el->get("b").toStdString(engine) == "class[B]");
    });

    delete engine;
}

void LvImportsTest::samePathImportTest()
{
    Engine* engine = new Engine;
    engine->setModuleFileType(Engine::Lv);
    std::string scriptsPath = lv::ApplicationContext::instance().releasePath() + "/data/ImportTest02.lv";
    std::string testPath = (QCoreApplication::applicationDirPath() + "/test").toStdString();


    TestPack tp(testPath);
    tp.unpack(scriptsPath);

    engine->scope([engine, testPath](){
        Object exports = engine->loadFile(testPath + "/main.lv");
        Object::Accessor localExports(exports);

        ScopedValue lv = localExports.get(engine, "main");

        QVERIFY(lv.isCallable());
        Callable c = lv.toCallable(engine);
        QVERIFY(c.isComponent());

        Component comp = c.toComponent();
        Element* el = comp.create(Function::Parameters(0));

        QVERIFY(el != nullptr);
        QVERIFY(el->get("a").toStdString(engine) == "class[A]");
        QVERIFY(el->get("b").toStdString(engine) == "class[B]");
    });


    delete engine;
}

void LvImportsTest::importPluginWithSamePathImportTest()
{
    Engine* engine = new Engine;
    engine->setModuleFileType(Engine::Lv);
    std::string scriptsPath = lv::ApplicationContext::instance().releasePath() + "/data/ImportTest03.lv";
    std::string testPath = (QCoreApplication::applicationDirPath() + "/test").toStdString();


    TestPack tp(testPath);
    tp.unpack(scriptsPath);

    engine->scope([engine, testPath](){
        Object exports = engine->loadFile(testPath + "/main.lv");
        Object::Accessor localExports(exports);

        ScopedValue lv = localExports.get(engine, "main");

        QVERIFY(lv.isCallable());
        Callable c = lv.toCallable(engine);
        QVERIFY(c.isComponent());

        Component comp = c.toComponent();
        Element* el = comp.create(Function::Parameters(0));

        QVERIFY(el != nullptr);
        QVERIFY(el->get("b").toStdString(engine) == "class[A]");
    });


    delete engine;
}

void LvImportsTest::importPluginThatImportsPlugin()
{
    Engine* engine = new Engine;
    engine->setModuleFileType(Engine::JsOnly);
    std::string scriptsPath = lv::ApplicationContext::instance().releasePath() + "/data/ImportTest04.lv";
    std::string testPath = (QCoreApplication::applicationDirPath() + "/test").toStdString();

    TestPack tp(testPath);
    tp.unpack(scriptsPath);

    engine->scope([engine, testPath](){
        Object exports = engine->loadFile(testPath + "/main.lv");
        Object::Accessor localExports(exports);

        ScopedValue lv = localExports.get(engine, "main");

        QVERIFY(lv.isCallable());
        Callable c = lv.toCallable(engine);
        QVERIFY(c.isComponent());

        Component comp = c.toComponent();
        Element* el = comp.create(Function::Parameters(0));

        QVERIFY(el != nullptr);
        QVERIFY(el->get("c").toStdString(engine) == "class[B]");
    });


    delete engine;
}

void LvImportsTest::samePathSingletonTest()
{
    Engine* engine = new Engine;
    engine->setModuleFileType(Engine::Lv);
    std::string scriptsPath = lv::ApplicationContext::instance().releasePath() + "/data/ImportTest05.lv";
    std::string testPath = (QCoreApplication::applicationDirPath() + "/test").toStdString();

    TestPack tp(testPath);
    tp.unpack(scriptsPath);

    engine->scope([engine, testPath](){
        Object exports = engine->loadFile(testPath + "/main.lv");
        Object::Accessor localExports(exports);

        ScopedValue lv = localExports.get(engine, "main");

        QVERIFY(lv.isCallable());
        Callable c = lv.toCallable(engine);
        QVERIFY(c.isComponent());

        Component comp = c.toComponent();
        Element* el = comp.create(Function::Parameters(0));

        QVERIFY(el != nullptr);
        QVERIFY(el->get("a").toStdString(engine) == "class[A]");
    });


    delete engine;
}

void LvImportsTest::moduleFileDependencyCycleTest()
{
    Engine* engine = new Engine;
    engine->setModuleFileType(Engine::Lv);
    std::string scriptsPath = lv::ApplicationContext::instance().releasePath() + "/data/ImportTest07.lv";
    std::string testPath = (QCoreApplication::applicationDirPath() + "/test").toStdString();

    TestPack tp(testPath);
    tp.unpack(scriptsPath);

    engine->scope([engine, testPath](){
        bool hadException = false;
        engine->tryCatch([engine, testPath](){
            engine->loadFile(testPath + "/main.lv");
        }, [&hadException](const Engine::CatchData& cd){
            QVERIFY(cd.message().find("Module file dependency cycle found") != std::string::npos);
            hadException = true;
        });
        QVERIFY(hadException);
    });

    delete engine;
}

void LvImportsTest::pluginDependencyCycleTest()
{
    Engine* engine = new Engine;
    engine->setModuleFileType(Engine::Lv);
    std::string scriptsPath = lv::ApplicationContext::instance().releasePath() + "/data/ImportTest08.lv";
    std::string testPath = (QCoreApplication::applicationDirPath() + "/test").toStdString();

    TestPack tp(testPath);
    tp.unpack(scriptsPath);

    engine->scope([engine, testPath](){
        bool hadException = false;
        engine->tryCatch([engine, testPath](){
            engine->loadFile(testPath + "/plugin1/subplugina/A.lv");
        }, [&hadException](const Engine::CatchData& cd){
            QVERIFY(cd.message().find("Plugin dependency cycle found") != std::string::npos);
            hadException = true;
        });
        QVERIFY(hadException);
    });

    delete engine;
}

void LvImportsTest::packageImportTest()
{
    Engine* engine = new Engine;
    engine->setModuleFileType(Engine::Lv);
    std::string scriptsPath = lv::ApplicationContext::instance().releasePath() + "/data/ImportTest09.lv";
    std::string testPath = (QCoreApplication::applicationDirPath() + "/test").toStdString();

    TestPack tp(testPath);
    tp.unpack(scriptsPath);

    engine->scope([engine, testPath](){
        engine->setPackageImportPaths({testPath + "/packages"});
        Object exports = engine->loadFile(testPath + "/main.lv");
        Object::Accessor localExports(exports);

        ScopedValue lv = localExports.get(engine, "main");

        QVERIFY(lv.isCallable());
        Callable c = lv.toCallable(engine);
        QVERIFY(c.isComponent());

        Component comp = c.toComponent();
        Element* el = comp.create(Function::Parameters(0));

        QVERIFY(el != nullptr);
        QVERIFY(el->get("a").toStdString(engine) == "class[A]");
    });

    delete engine;
}

void LvImportsTest::importAsTest()
{
    Engine* engine = new Engine;
    engine->setModuleFileType(Engine::Lv);
    std::string scriptsPath = lv::ApplicationContext::instance().releasePath() + "/data/ImportTest11.lv";
    std::string testPath = (QCoreApplication::applicationDirPath() + "/test").toStdString();

    TestPack tp(testPath);
    tp.unpack(scriptsPath);

    engine->scope([engine, testPath](){
        Object exports = engine->loadFile(testPath + "/main.lv");

        Object::Accessor localExports(exports);
        ScopedValue lv = localExports.get(engine, "main");

        QVERIFY(lv.isCallable());
        Callable c = lv.toCallable(engine);
        QVERIFY(c.isComponent());

        Component comp = c.toComponent();
        Element* el = comp.create(Function::Parameters(0));

        QVERIFY(el != nullptr);
        QVERIFY(el->get("a").toStdString(engine) == "class[A]");
        QVERIFY(el->get("b").toStdString(engine) == "class[B]");
    });

    delete engine;
}


