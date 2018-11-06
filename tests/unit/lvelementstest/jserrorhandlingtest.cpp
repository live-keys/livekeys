#include "jserrorhandlingtest.h"
#include "live/elements/element.h"
#include "live/elements/engine.h"
#include "live/exception.h"

Q_TEST_RUNNER_REGISTER(JsErrorHandlingTest);

using namespace lv;
using namespace lv::el;

class JsErrorHandlingStub : public Element{

    META_OBJECT{
        META_OBJECT_DESCRIBE(JsErrorHandlingStub)
            .base<Element>()
            .constructor<>()
            .scriptFunction("errorFunction", &JsErrorHandlingStub::errorFunction)
            .scriptMethod("errorTriggered", &JsErrorHandlingStub::errorTriggered)
            .scriptMethod("errorMethod", &JsErrorHandlingStub::errorMethod)
            .scriptProperty<int>("errorProperty",
                                 &JsErrorHandlingStub::errorProperty,
                                 &JsErrorHandlingStub::setErrorProperty,
                                 "errorPropertyChanged")
            .scriptEvent("errorPropertyChanged", &JsErrorHandlingStub::errorPropertyChanged)
        META_OBJECT_CLOSE
    }

public:
    JsErrorHandlingStub(Engine* engine) : Element(engine), m_errorTriggered(false){}

    static void errorFunction(const Function::CallInfo& callInfo){
        lv::Exception e = CREATE_EXCEPTION(lv::Exception, "TestException", 20);
        callInfo.engine()->throwError(&e, nullptr);
    }

    void errorMethod(){
        lv::Exception e = CREATE_EXCEPTION(lv::Exception, "TestException", 20);
        engine()->throwError(&e, this);
    }

    void errorTriggered(){ m_errorTriggered = true; }

    int errorProperty() const{ return 20; }
    void setErrorProperty(int){
        lv::Exception e = CREATE_EXCEPTION(lv::Exception, "TestException", 20);
        engine()->throwError(&e, this);
    }

    Event errorPropertyChanged(){
        static Event::Id eid = eventId(&JsErrorHandlingStub::errorPropertyChanged);
        return notify(eid);
    }

    bool m_errorTriggered;

};

JsErrorHandlingTest::JsErrorHandlingTest(QObject* parent)
    : QObject(parent)
{

}

void JsErrorHandlingTest::initTestCase(){
}

void JsErrorHandlingTest::testException(){
    Engine* engine = new Engine;

    engine->scope([engine](){
        engine->tryCatch([engine](){
            engine->compileEnclosed("throw new Error('Test Js Exception', 'testfile', 12);")->run();
        }, [engine](const Engine::CatchData& cd){
            QVERIFY(cd.message() == "Test Js Exception");
        });
    });

    delete engine;
}

void JsErrorHandlingTest::testExceptionWithLink(){
    Engine* engine = new Engine;

    engine->scope([engine](){
        JsErrorHandlingStub* jsError = new JsErrorHandlingStub(engine);
        jsError->ref();

        LocalObject globalObject(engine->currentContext());
        globalObject.set(engine, "jsError", LocalValue(engine, jsError));

        engine->tryCatch([engine](){
            engine->compileEnclosed("throw linkError(new Error('Test Js Exception'), jsError)")->run();
        }, [engine, jsError](const Engine::CatchData& cd){
            QVERIFY(cd.message() == "Test Js Exception");
            QVERIFY(cd.object() == jsError);
        });
    });

    delete engine;
}

void JsErrorHandlingTest::testExceptionFromFunction(){
    Engine* engine = new Engine;

    Module::Ptr m = Module::createLoader("test", 1, 0);
    m->addType<JsErrorHandlingStub>();

    engine->scope([engine, m](){
        Object o  = engine->require(m);
        LocalObject lo(o);

        JsErrorHandlingStub* jsError = new JsErrorHandlingStub(engine);
        jsError->ref();

        LocalObject globalObject(engine->currentContext());
        globalObject.set(engine, "JsErrorHandlingStub", lo.get(engine, "JsErrorHandlingStub"));
        globalObject.set(engine, "jsError", LocalValue(engine, jsError));


        engine->tryCatch([engine](){
            engine->compileEnclosed("JsErrorHandlingStub.errorFunction();")->run();
        }, [engine](const Engine::CatchData& cd){
            QVERIFY(cd.fileName().find("errorhandlingtest.cpp") != std::string::npos);
            QVERIFY(cd.message() == "TestException");
            QVERIFY(cd.object() == nullptr);
        });

        delete jsError;
    });

    delete engine;
}

void JsErrorHandlingTest::testExceptionFromMethod()
{
    Engine* engine = new Engine;

    Module::Ptr m = Module::createLoader("test", 1, 0);
    m->addType<JsErrorHandlingStub>();

    engine->scope([engine, m](){
        Object o  = engine->require(m);
        LocalObject lo(o);

        JsErrorHandlingStub* jsError = new JsErrorHandlingStub(engine);
        jsError->ref();

        LocalObject globalObject(engine->currentContext());
        globalObject.set(engine, "JsErrorHandlingStub", lo.get(engine, "JsErrorHandlingStub"));
        globalObject.set(engine, "jsError", LocalValue(engine, jsError));


        engine->tryCatch([engine](){
            engine->compileEnclosed("jsError.errorMethod();")->run();
        }, [engine, jsError](const Engine::CatchData& cd){
            QVERIFY(cd.fileName().find("errorhandlingtest.cpp") != std::string::npos);
            QVERIFY(cd.message() == "TestException");
            QVERIFY(cd.object() == jsError);
        });

        delete jsError;
    });

    delete engine;
}

void JsErrorHandlingTest::testExceptionFromProperty()
{
    Engine* engine = new Engine;

    Module::Ptr m = Module::createLoader("test", 1, 0);
    m->addType<JsErrorHandlingStub>();

    engine->scope([engine, m](){
        Object o  = engine->require(m);
        LocalObject lo(o);

        JsErrorHandlingStub* jsError = new JsErrorHandlingStub(engine);
        jsError->ref();

        LocalObject globalObject(engine->currentContext());
        globalObject.set(engine, "JsErrorHandlingStub", lo.get(engine, "JsErrorHandlingStub"));
        globalObject.set(engine, "jsError", LocalValue(engine, jsError));


        engine->tryCatch([engine](){
            engine->compileEnclosed("jsError.errorProperty = 100;")->run();
        }, [engine, jsError](const Engine::CatchData& cd){
            QVERIFY(cd.fileName().find("errorhandlingtest.cpp") != std::string::npos);
            QVERIFY(cd.message() == "TestException");
            QVERIFY(cd.object() == jsError);
        });

        delete jsError;
    });

    delete engine;
}

void JsErrorHandlingTest::testExceptionPropagation(){

    Engine* engine = new Engine;

    Module::Ptr m = Module::createLoader("test", 1, 0);
    m->addType<JsErrorHandlingStub>();

    engine->scope([engine, m](){
        Object o  = engine->require(m);
        LocalObject lo(o);

        JsErrorHandlingStub* jsError = new JsErrorHandlingStub(engine);
        jsError->ref();

        LocalObject globalObject(engine->currentContext());
        globalObject.set(engine, "JsErrorHandlingStub", lo.get(engine, "JsErrorHandlingStub"));
        globalObject.set(engine, "jsError", LocalValue(engine, jsError));

        engine->compileElement(
            "var c = new Container();"
            "var eh = new ErrorHandler();"
            "eh.setParent(c);"
            "eh.target = c;"
            "eh.on('error', function(){ jsError.errorTriggered(); });"
            "var es = new JsErrorHandlingStub();"
            "es.setParent(c);"
            "es.errorMethod();"
        )->loadAsModule();

        QVERIFY(jsError->m_errorTriggered);

        delete jsError;
    });

    delete engine;
}

void JsErrorHandlingTest::testExceptionPropagationFromJs(){
    Engine* engine = new Engine;

    Module::Ptr m = Module::createLoader("test", 1, 0);
    m->addType<JsErrorHandlingStub>();

    engine->scope([engine, m](){
        Object o  = engine->require(m);
        LocalObject lo(o);

        JsErrorHandlingStub* jsError = new JsErrorHandlingStub(engine);
        jsError->ref();

        LocalObject globalObject(engine->currentContext());
        globalObject.set(engine, "JsErrorHandlingStub", lo.get(engine, "JsErrorHandlingStub"));
        globalObject.set(engine, "jsError", LocalValue(engine, jsError));

        engine->compileElement(
            "var c = new Container();"
            "var eh = new ErrorHandler();"
            "eh.setParent(c);"
            "eh.target = c;"
            "eh.on('error', function(){ jsError.errorTriggered(); });"
            "var es = new JsErrorHandlingStub();"
            "es.setParent(c);"
            "throw linkError(new Error('TestError'), es);"
        )->loadAsModule();

        QVERIFY(jsError->m_errorTriggered);

        delete jsError;
    });

    delete engine;
}

void JsErrorHandlingTest::testExceptionRethrowPropagation()
{
    Engine* engine = new Engine;

    Module::Ptr m = Module::createLoader("test", 1, 0);
    m->addType<JsErrorHandlingStub>();

    engine->scope([engine, m](){
        Object o  = engine->require(m);
        LocalObject lo(o);

        JsErrorHandlingStub* jsError = new JsErrorHandlingStub(engine);
        jsError->ref();

        LocalObject globalObject(engine->currentContext());
        globalObject.set(engine, "JsErrorHandlingStub", lo.get(engine, "JsErrorHandlingStub"));
        globalObject.set(engine, "jsError", LocalValue(engine, jsError));

        // Container c1
        //   ErrorHandler e1
        //   Container c2
        //      ErrorHandler e2 (rethrows)
        //      ErrorTrigger

        engine->compileElement(
            "var c1 = new Container();"
            "var e1 = new ErrorHandler();"
            "e1.setParent(c1);"
            "e1.target = c1;"
            "e1.on('error', function(){ jsError.errorTriggered(); });"

            "var c2 = new Container();"
            "c2.setParent(c1);"
            "var e2 = new ErrorHandler();"
            "e2.setParent(c2);"
            "e2.target = c2;"
            "e2.on('error', function(e){ e2.rethrow(e); });"
            "var es = new JsErrorHandlingStub();"
            "es.setParent(c2);"
            "es.errorMethod();"
        )->loadAsModule();

        QVERIFY(jsError->m_errorTriggered);

        delete jsError;
    });

    delete engine;
}
