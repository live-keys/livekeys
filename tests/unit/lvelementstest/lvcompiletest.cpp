#include "lvcompiletest.h"
#include "live/elements/engine.h"
#include "live/elements/element.h"
#include "live/elements/metaobject.h"
#include "live/elements/modulelibrary.h"
#include "live/elements/component.h"
#include "live/elements/container.h"
#include "live/elements/object.h"
#include "live/elements/compiler/modulefile.h"
#include "live/mlnodetojson.h"
#include "live/applicationcontext.h"
#include "testpack.h"

Q_TEST_RUNNER_REGISTER(LvCompileTest);

using namespace lv;
using namespace lv::el;

LvCompileTest::LvCompileTest(QObject *parent)
    : QObject(parent)
{
}

void LvCompileTest::initTestCase(){
}

std::string LvCompileTest::scriptPath(const std::string &scriptName){
    return Path::parent(lv::ApplicationContext::instance().applicationFilePath()) + "/data/" + scriptName;
}

std::string LvCompileTest::testPath(){
    return Path::parent(lv::ApplicationContext::instance().applicationFilePath()) + "/test";
}

void LvCompileTest::compileModule(){
    TestPack tp(testPath(), true);
    tp.unpack(scriptPath("ImportTest01.lvep"));

    Compiler::Config compilerConfig;
    Compiler::Ptr compiler = Compiler::create(compilerConfig);
    compiler->configureImplicitType("console");
    compiler->configureImplicitType("vlog");

    ElementsModule::Ptr em = Compiler::compileModule(compiler, tp.path() + "/plugin1");
    ModuleFile* mfA = em->moduleFileBypath(tp.path() + "/plugin1/A.lv");
    QVERIFY(mfA != nullptr);
    QVERIFY(mfA->jsFilePath() == tp.path() + "/plugin1/A.lv.js");
    ModuleFile* mfB = em->moduleFileBypath(tp.path() + "/plugin1/B.lv");
    QVERIFY(mfB != nullptr);
    QVERIFY(mfB->jsFilePath() == tp.path() + "/plugin1/B.lv.js");
}

void LvCompileTest::compilePackage(){
    TestPack tp(testPath(), true);
    tp.unpack(scriptPath("ImportTest01.lvep"));

    Compiler::Config compilerConfig;
    Compiler::Ptr compiler = Compiler::create(compilerConfig);
    compiler->configureImplicitType("console");
    compiler->configureImplicitType("vlog");

    auto ems = Compiler::compilePackage(compiler, tp.path());
    QVERIFY(ems.size() == 2);
    auto mainEm = ems.front()->module()->path() == tp.path() ? ems.front() : ems.back();
    auto internalEm = ems.front()->module()->path() == tp.path() ? ems.back() : ems.front();

    QVERIFY(mainEm->module()->path() == tp.path());
    QVERIFY(internalEm->module()->path() == Path::join(tp.path(), "plugin1"));
    ModuleFile* mfA = internalEm->moduleFileBypath(tp.path() + "/plugin1/A.lv");
    QVERIFY(mfA != nullptr);
    QVERIFY(mfA->jsFilePath() == tp.path() + "/plugin1/A.lv.js");
    ModuleFile* mfB = internalEm->moduleFileBypath(tp.path() + "/plugin1/B.lv");
    QVERIFY(mfB != nullptr);
    QVERIFY(mfB->jsFilePath() == tp.path() + "/plugin1/B.lv.js");
}

void LvCompileTest::compilePackageWithRelease(){
    TestPack tp(testPath(), true);
    tp.unpack(scriptPath("CompileTest01.lvep"));

    Compiler::Config compilerConfig;
    compilerConfig.initialize({{"packageBuildPath", "build"}});
    Compiler::Ptr compiler = Compiler::create(compilerConfig);
    compiler->configureImplicitType("console");
    compiler->configureImplicitType("vlog");
    compiler->setPackageImportPaths({tp.path() + "/packages"});

    std::string packagePath = Path::join(Path::join(tp.path(), "packages"), "package1");
    std::string packageFilePath = Path::join(packagePath, Package::fileName);

    FileIO fio;
    std::string packageFileData = fio.readFromFile(packageFilePath);
    MLNode packageData;
    ml::fromJson(packageFileData, packageData);
    packageData["release"] = "build";
    ml::toJson(packageData, packageFileData);
    QVERIFY(fio.writeToFile(packageFilePath, packageFileData));

    // compile released without having anything build, should throw exception

    bool hasException = false;
    try{
        Compiler::compileModule(compiler, packagePath);
    } catch ( lv::Exception& e ){
        QVERIFY(e.message().find("Released package") != std::string::npos );
        QVERIFY(e.code() == Exception::toCode("~File"));
        hasException = true;
    }
    QVERIFY(hasException);

    hasException = false;
    try{
        Compiler::compile(compiler, packagePath + "/A.lv");
    } catch ( lv::Exception& e ){
        QVERIFY(e.message().find("Released package") != std::string::npos );
        QVERIFY(e.code() == Exception::toCode("~File"));
        hasException = true;
    }
    QVERIFY(hasException);

    // build the module

    packageData["release"] = "";
    ml::toJson(packageData, packageFileData);
    QVERIFY(fio.writeToFile(packageFilePath, packageFileData));
    auto em = Compiler::compileModule(compiler, packagePath);

    ModuleFile* mfA = em->moduleFileBypath(packagePath + "/A.lv");
    QVERIFY(mfA != nullptr);
    QVERIFY(mfA->jsFilePath() == packagePath + "/build/A.lv.js");

    // compile released with the build folder ready

    packageData["release"] = "build";
    ml::toJson(packageData, packageFileData);
    QVERIFY(fio.writeToFile(packageFilePath, packageFileData));
    em = Compiler::compileModule(compiler, packagePath);

    ModuleFile* mfcA = em->moduleFileBypath(packagePath + "/A.lv");
    QVERIFY(mfcA != nullptr);
    QVERIFY(mfcA->jsFilePath() == packagePath + "/build/A.lv.js");
}


void LvCompileTest::test1Lv(){
    lv::el::Engine* engine = new lv::el::Engine(Compiler::create(Compiler::Config(true, ".test.js")));
    engine->setModuleFileType(Engine::Lv);
    engine->scope([engine](){
        std::string scriptsPath = lv::ApplicationContext::instance().releasePath() + "/data/Test1.lv";
        ElementsModule::Ptr epl = Compiler::compile(engine->compiler(), scriptsPath, engine);

        ModuleFile* mf = epl->moduleFileBypath(scriptsPath);
        JsModule::Ptr jsMod = engine->loadJsModule(mf->jsFilePath());
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
    lv::el::Engine* engine = new lv::el::Engine(Compiler::create(Compiler::Config(true, ".test.js")));
    engine->scope([engine](){
        std::string scriptsPath = lv::ApplicationContext::instance().releasePath() + "/data/Test2.lv";
        ElementsModule::Ptr epl = Compiler::compile(engine->compiler(), scriptsPath, engine);

        ModuleFile* mf = epl->moduleFileBypath(scriptsPath);
        JsModule::Ptr jsMod = engine->loadJsModule(mf->jsFilePath());
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

        QVERIFY(elem->get("children").isArray());
        Object::ArrayAccessor oaa(engine, elem->get("children"));
        QVERIFY(oaa.length() == 1);

        Element* child0 = oaa.get(engine, 0).toElement(engine);
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
        ElementsModule::Ptr epl = Compiler::compile(engine->compiler(), scriptsPath, engine);

        ModuleFile* mf = epl->moduleFileBypath(scriptsPath);
        JsModule::Ptr jsMod = engine->loadJsModule(mf->jsFilePath());
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

        QVERIFY(elem->get("children").isArray());
        Object::ArrayAccessor oaa(engine, elem->get("children"));
        QVERIFY(oaa.length() == 1);

        Element* child0 = oaa.get(engine, 0).toElement(engine);
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
        ElementsModule::Ptr epl = Compiler::compile(engine->compiler(), scriptsPath, engine);

        ModuleFile* mf = epl->moduleFileBypath(scriptsPath);
        JsModule::Ptr jsMod = engine->loadJsModule(mf->jsFilePath());
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


        QVERIFY(elem->get("children").isArray());
        Object::ArrayAccessor oaa(engine, elem->get("children"));
        QVERIFY(oaa.length() == 2);

        Element* child0 = oaa.get(engine, 0).toElement(engine);
        QVERIFY(child0->hasProperty("k"));
        QVERIFY(child0->get("k").toInt32(engine) == 20);

        Element* child1 = oaa.get(engine, 1).toElement(engine);
        QVERIFY(child1->hasProperty("message"));
        QVERIFY(child1->get("message").toStdString(engine) == "thirty");
    });

    delete engine;
}

void LvCompileTest::test5Lv(){
    lv::el::Engine* engine = new lv::el::Engine();
    engine->scope([engine](){
        std::string scriptsPath = lv::ApplicationContext::instance().releasePath() + "/data/Test5.lv";
        ElementsModule::Ptr epl = Compiler::compile(engine->compiler(), scriptsPath, engine);

        ModuleFile* mf = epl->moduleFileBypath(scriptsPath);
        JsModule::Ptr jsMod = engine->loadJsModule(mf->jsFilePath());
        jsMod->evaluate();

        ScopedValue sv = jsMod->moduleNamespace();
        Object::Accessor oa(sv);

        ScopedValue rootValue = oa.get(engine, "main");
        QVERIFY(rootValue.isElement());
        Element* elem = rootValue.toElement(engine);
        QVERIFY(elem->get("x").toInt32(engine) == 200);
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
