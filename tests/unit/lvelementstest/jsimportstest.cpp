#include "jsimportstest.h"
#include "live/elements/engine.h"
#include "live/elements/element.h"
#include "live/elements/metaobject.h"
#include "live/elements/modulelibrary.h"
#include "live/elements/component.h"
#include "live/elements/container.h"
#include "live/applicationcontext.h"
#include "live/exception.h"

#include "testpack.h"

Q_TEST_RUNNER_REGISTER(JsImportsTest);

using namespace lv;
using namespace lv::el;

JsImportsTest::JsImportsTest(QObject *parent)
    : QObject(parent)
{
}

void JsImportsTest::initTestCase(){

}

void JsImportsTest::singlePluginImportTest(){
    Engine* engine = new Engine;
    engine->setModuleFileType(Engine::JsOnly);
    std::string scriptsPath = lv::ApplicationContext::instance().releasePath() + "/data/ImportTest01.lv";
    std::string testPath = (QCoreApplication::applicationDirPath() + "/test").toStdString();

    TestPack tp(testPath);
    tp.unpack(scriptsPath);

    engine->scope([engine, testPath](){
        Object exports = engine->loadJsModule(testPath + "/main.lv.js");
        LocalObject localExports(exports);

        LocalValue lv = localExports.get(engine, "main");

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


void JsImportsTest::samePathImportTest(){
    Engine* engine = new Engine;
    engine->setModuleFileType(Engine::JsOnly);
    std::string scriptsPath = lv::ApplicationContext::instance().releasePath() + "/data/ImportTest02.lv";
    std::string testPath = (QCoreApplication::applicationDirPath() + "/test").toStdString();


    TestPack tp(testPath);
    tp.unpack(scriptsPath);

    engine->scope([engine, testPath](){
        Object exports = engine->loadJsModule(testPath + "/main.lv.js");
        LocalObject localExports(exports);

        LocalValue lv = localExports.get(engine, "main");

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

void JsImportsTest::importPluginWithSamePathImportTest(){
    Engine* engine = new Engine;
    engine->setModuleFileType(Engine::JsOnly);
    std::string scriptsPath = lv::ApplicationContext::instance().releasePath() + "/data/ImportTest03.lv";
    std::string testPath = (QCoreApplication::applicationDirPath() + "/test").toStdString();

    TestPack tp(testPath);
    tp.unpack(scriptsPath);

    engine->scope([engine, testPath](){
        Object exports = engine->loadJsModule(testPath + "/main.lv.js");
        LocalObject localExports(exports);

        LocalValue lv = localExports.get(engine, "main");

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

void JsImportsTest::packageImportTest(){
    Engine* engine = new Engine;
    engine->setModuleFileType(Engine::JsOnly);
    std::string scriptsPath = lv::ApplicationContext::instance().releasePath() + "/data/ImportTest09.lv";
    std::string testPath = (QCoreApplication::applicationDirPath() + "/test").toStdString();

    TestPack tp(testPath);
    tp.unpack(scriptsPath);

    engine->scope([engine, testPath](){
        engine->setPackageImportPaths({testPath + "/packages"});
        Object exports = engine->loadJsModule(testPath + "/main.lv.js");
        LocalObject localExports(exports);

        LocalValue lv = localExports.get(engine, "main");

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

void JsImportsTest::importPluginThatImportsPlugin(){
    Engine* engine = new Engine;
    engine->setModuleFileType(Engine::JsOnly);
    std::string scriptsPath = lv::ApplicationContext::instance().releasePath() + "/data/ImportTest04.lv";
    std::string testPath = (QCoreApplication::applicationDirPath() + "/test").toStdString();

    TestPack tp(testPath);
    tp.unpack(scriptsPath);

    engine->scope([engine, testPath](){
        Object exports = engine->loadJsModule(testPath + "/main.lv.js");
        LocalObject localExports(exports);

        LocalValue lv = localExports.get(engine, "main");

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


void JsImportsTest::samePathSingletonTest(){
    Engine* engine = new Engine;
    engine->setModuleFileType(Engine::JsOnly);
    std::string scriptsPath = lv::ApplicationContext::instance().releasePath() + "/data/ImportTest05.lv";
    std::string testPath = (QCoreApplication::applicationDirPath() + "/test").toStdString();

    TestPack tp(testPath);
    tp.unpack(scriptsPath);

    engine->scope([engine, testPath](){
        Object exports = engine->loadJsModule(testPath + "/main.lv.js");
        LocalObject localExports(exports);

        LocalValue lv = localExports.get(engine, "main");

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

void JsImportsTest::invalidExportTypeAsObjectTest(){
    Engine* engine = new Engine;
    engine->setModuleFileType(Engine::JsOnly);
    std::string scriptsPath = lv::ApplicationContext::instance().releasePath() + "/data/ImportTest06.lv";
    std::string testPath = (QCoreApplication::applicationDirPath() + "/test").toStdString();

    TestPack tp(testPath);
    tp.unpack(scriptsPath);

    engine->scope([engine, testPath](){
        engine->tryCatch([engine, testPath](){
            engine->loadJsModule(testPath + "/A.lv.js");
        }, [engine](const Engine::CatchData& cd){
            QVERIFY(cd.message().find("Export at 'A' is not a component.") != std::string::npos);
        });
    });

    delete engine;
}

void JsImportsTest::invalidExportTypeAsStringTest(){
    Engine* engine = new Engine;
    engine->setModuleFileType(Engine::JsOnly);
    std::string scriptsPath = lv::ApplicationContext::instance().releasePath() + "/data/ImportTest06.lv";
    std::string testPath = (QCoreApplication::applicationDirPath() + "/test").toStdString();

    TestPack tp(testPath);
    tp.unpack(scriptsPath);

    engine->scope([engine, testPath](){
        engine->tryCatch([engine, testPath](){
            engine->loadJsModule(testPath + "/B.lv.js");
        }, [](const Engine::CatchData& cd){
            QVERIFY(cd.message().find("Export at 'B' is not a component nor a singleton.") != std::string::npos);
        });
    });

    delete engine;
}

void JsImportsTest::invalidExportKeyTest(){
    Engine* engine = new Engine;
    engine->setModuleFileType(Engine::JsOnly);
    std::string scriptsPath = lv::ApplicationContext::instance().releasePath() + "/data/ImportTest06.lv";
    std::string testPath = (QCoreApplication::applicationDirPath() + "/test").toStdString();

    TestPack tp(testPath);
    tp.unpack(scriptsPath);

    engine->scope([engine, testPath](){
        engine->tryCatch([engine, testPath](){
            engine->loadJsModule(testPath + "/C.lv.js");
        }, [](const Engine::CatchData& cd){
            QVERIFY(cd.message().find("Given an export key that is not of string type.") != std::string::npos);
        });
    });

    delete engine;
}

void JsImportsTest::indirectInvalidExportTypeAsObjectTest(){
    Engine* engine = new Engine;
    engine->setModuleFileType(Engine::JsOnly);
    std::string scriptsPath = lv::ApplicationContext::instance().releasePath() + "/data/ImportTest06.lv";
    std::string testPath = (QCoreApplication::applicationDirPath() + "/test").toStdString();

    TestPack tp(testPath);
    tp.unpack(scriptsPath);

    engine->scope([engine, testPath](){
        engine->tryCatch([engine, testPath](){
            engine->loadJsModule(testPath + "/main.lv.js");
        }, [](const Engine::CatchData& cd){
            QVERIFY(cd.message().find("Export at 'A' is not a component.") != std::string::npos);
        });
    });

    delete engine;
}

void JsImportsTest::moduleFileDependencyCycleTest(){
    Engine* engine = new Engine;
    engine->setModuleFileType(Engine::JsOnly);
    std::string scriptsPath = lv::ApplicationContext::instance().releasePath() + "/data/ImportTest07.lv";
    std::string testPath = (QCoreApplication::applicationDirPath() + "/test").toStdString();

    TestPack tp(testPath);
    tp.unpack(scriptsPath);

    engine->scope([engine, testPath](){
        bool hadException = false;
        engine->tryCatch([engine, testPath](){
            engine->loadJsModule(testPath + "/main.lv.js");
        }, [&hadException](const Engine::CatchData& cd){
            QVERIFY(cd.message().find("Module file dependency cycle found") != std::string::npos);
            hadException = true;
        });
        QVERIFY(hadException);
    });

    delete engine;
}


void JsImportsTest::pluginDependencyCycleTest(){
    Engine* engine = new Engine;
    engine->setModuleFileType(Engine::JsOnly);
    std::string scriptsPath = lv::ApplicationContext::instance().releasePath() + "/data/ImportTest08.lv";
    std::string testPath = (QCoreApplication::applicationDirPath() + "/test").toStdString();

    TestPack tp(testPath);
    tp.unpack(scriptsPath);

    engine->scope([engine, testPath](){
        bool hadException = false;
        engine->tryCatch([engine, testPath](){
            engine->loadJsModule(testPath + "/plugin1/subplugina/A.lv.js");
        }, [&hadException](const Engine::CatchData& cd){
            QVERIFY(cd.message().find("Plugin dependency cycle found") != std::string::npos);
            hadException = true;
        });
        QVERIFY(hadException);
    });

    delete engine;
}

void JsImportsTest::packageDependencyCycleTest(){
    Engine* engine = new Engine;
    engine->setModuleFileType(Engine::JsOnly);
    std::string scriptsPath = lv::ApplicationContext::instance().releasePath() + "/data/ImportTest10.lv";
    std::string testPath = (QCoreApplication::applicationDirPath() + "/test").toStdString();

    TestPack tp(testPath);
    tp.unpack(scriptsPath);

    engine->scope([engine, testPath](){
        engine->setPackageImportPaths({testPath + "/packages"});
        bool hadException = false;
        engine->tryCatch([engine, testPath](){
            engine->loadJsModule(testPath + "/main.lv.js");
        }, [&hadException](const Engine::CatchData& cd){
            QVERIFY(cd.message().find("Package dependency cycle found") != std::string::npos);
            hadException = true;
        });
        QVERIFY(hadException);
    });

    delete engine;
}

void JsImportsTest::importAsTest(){
    Engine* engine = new Engine;
    engine->setModuleFileType(Engine::JsOnly);
    std::string scriptsPath = lv::ApplicationContext::instance().releasePath() + "/data/ImportTest11.lv";
    std::string testPath = (QCoreApplication::applicationDirPath() + "/test").toStdString();

    TestPack tp(testPath);
    tp.unpack(scriptsPath);

    engine->scope([engine, testPath](){
        Object exports = engine->loadJsModule(testPath + "/main.lv.js");

        LocalObject localExports(exports);
        LocalValue lv = localExports.get(engine, "main");

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
