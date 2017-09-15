#include "enginetest.h"
#include "engineteststub.h"
#include "live/errorhandler.h"
#include "live/exception.h"
#include "live/engine.h"
#include <QVariant>
#include <QQmlEngine>
#include <QQmlContext>

Q_TEST_RUNNER_REGISTER(EngineTest);

using namespace lcv;

EngineTest::EngineTest(QObject *parent)
    : QObject(parent)
{
}

void EngineTest::initTestCase(){
    qmlRegisterType<EngineTestStub>("cv", 1, 0, "EngineTestStub");
    qmlRegisterType<lcv::ErrorHandler>("cv", 1, 0, "ErrorHandler");
}

void EngineTest::cppExceptionInObbjectTest(){
    Engine engine(new QQmlEngine);

    QObject* obj = engine.createObject(
        "import QtQuick 2.3\n import cv 1.0\n "
        "EngineTestStub{}",
        0,
        QUrl::fromLocalFile("enginetest.qml")
    );

    QVERIFY(obj != 0);

    bool isException = false;
    try{
        QMetaObject::invokeMethod(obj, "throwException");
        QCoreApplication::processEvents();
    } catch ( lcv::Exception& ){
        isException = true;
    }
    QVERIFY(isException);
}

void EngineTest::engineExceptionTest(){
    Engine engine(new QQmlEngine);

    lcv::Exception exception = lcv::Exception::create<lcv::Exception>(
        "JSTest", 1, "enginetest.cpp", 100, "jsExceptionInObjectTest", 0
    );

    bool isException = false;

    QObject::connect(&engine, &Engine::applicationError, [&isException, this](QJSValue error){
        QCOMPARE(error.property("type").toString(), QString("Exception"));
        QCOMPARE(error.property("message").toString(), QString("JSTest"));
        QCOMPARE(error.property("code").toInt(), 1);
        QCOMPARE(error.property("functionName").toString(), QString("jsExceptionInObjectTest"));
        QCOMPARE(error.property("fileName").toString(), QString("enginetest.cpp"));
        QCOMPARE(error.property("lineNumber").toInt(), 100);
        QCOMPARE(error.property("object").toQObject(), this);
        isException = true;
    });

    engine.throwError(&exception, this);
    QCoreApplication::processEvents();

    QVERIFY(isException);
}

void EngineTest::engineWarningTest(){
    Engine engine(new QQmlEngine);

    bool isWarning = false;

    QObject::connect(&engine, &Engine::applicationWarning, [&isWarning, this](QJSValue error){
        QCOMPARE(error.property("message").toString(), QString("JSTest"));
        QCOMPARE(error.property("fileName").toString(), QString("enginetest.cpp"));
        QCOMPARE(error.property("lineNumber").toInt(), 100);
        QCOMPARE(error.property("object").toQObject(), this);
        isWarning = true;
    });

    engine.throwWarning("JSTest", this, "enginetest.cpp", 100);
    QCoreApplication::processEvents();

    QVERIFY(isWarning);
}

void EngineTest::engineObjectExceptionTest(){
    Engine engine(new QQmlEngine);

    bool isException = false;
    QObject::connect(&engine, &Engine::applicationError, [&isException, this](QJSValue error){
        QCOMPARE(error.property("type").toString(), QString("Exception"));
        QCOMPARE(error.property("message").toString(), QString("JSTest"));
        QCOMPARE(error.property("code").toInt(), 1);
        QCOMPARE(error.property("functionName").toString(), QString("jsExceptionInObjectTest"));
        QCOMPARE(error.property("fileName").toString(), QString("enginetest.cpp"));
        QCOMPARE(error.property("lineNumber").toInt(), 100);
        isException = true;
    });

    engine.engine()->rootContext()->setContextProperty("engine", &engine);
    QObject* obj = engine.createObject(
        "import QtQuick 2.3\n import cv 1.0\n "
        "EngineTestStub{"
            "Component.onCompleted: throwJsError()"
        "}",
        0,
        QUrl::fromLocalFile("enginetest.qml")
    );

    QVERIFY(obj != 0);

    QCoreApplication::processEvents();

    QVERIFY(isException);
}

void EngineTest::engineInternalWarningTest(){
    Engine engine(new QQmlEngine);

    bool isWarning = false;
    QObject::connect(&engine, &Engine::applicationWarning, [&isWarning, this](QJSValue error){
        QCOMPARE(error.property("message").toString(), QString("JSTest"));
        QCOMPARE(error.property("fileName").toString(), QString("enginetest.cpp"));
        QCOMPARE(error.property("lineNumber").toInt(), 100);
        isWarning = true;
    });

    engine.engine()->rootContext()->setContextProperty("engine", &engine);
    QObject* obj = engine.createObject(
        "import QtQuick 2.3\n import cv 1.0\n "
        "EngineTestStub{"
            "Component.onCompleted: throwJsWarning()"
        "}",
        0,
        QUrl::fromLocalFile("enginetest.qml")
    );

    QVERIFY(obj != 0);

    QCoreApplication::processEvents();

    QVERIFY(isWarning);
}

void EngineTest::engineErrorHandlerTest(){
    Engine* engine = new Engine(new QQmlEngine);

    bool isWarning = false;
    bool isError   = false;
    QObject::connect(engine, &Engine::applicationWarning, [&isWarning, this](QJSValue){
        isWarning = true;
    });
    QObject::connect(engine, &Engine::applicationError, [&isError, this](QJSValue){
        isError = true;
    });

    QObject* livecvStub = new QObject;
    livecvStub->setProperty("engine", QVariant::fromValue(engine));

    engine->engine()->rootContext()->setContextProperty("engine", engine);
    engine->engine()->rootContext()->setContextProperty("livecv", livecvStub);
    QObject* obj = engine->createObject(
        "import QtQuick 2.3\n import cv 1.0\n "
        "Item{\n"
            "id: root;\n"
            "property string errorMessage: 'empty';\n"
            "property string warningMessage: 'empty';\n"
            "ErrorHandler{\n"
                "onError:   root.errorMessage   = e.message;\n"
                "onWarning: root.warningMessage = e.message;\n"
            "}\n"
            "EngineTestStub{\n"
                "Component.onCompleted: {\n"
                    "throwJsWarning();"
                    "throwJsError();"
                "}\n"
            "}\n"
        "}\n",
        0,
        QUrl::fromLocalFile("enginetest.qml")
    );

    QVERIFY(obj != 0);

    QCoreApplication::processEvents();

    QVERIFY(!isWarning);
    QVERIFY(!isError);

    QCOMPARE(obj->property("errorMessage").toString(),   QString("JSTest"));
    QCOMPARE(obj->property("warningMessage").toString(), QString("JSTest"));
}

void EngineTest::engineErrorHandlerSkipTest(){
    Engine* engine = new Engine(new QQmlEngine);

    bool isWarning = false;
    bool isError   = false;
    QObject::connect(engine, &Engine::applicationWarning, [&isWarning, this](QJSValue){
        isWarning = true;
    });
    QObject::connect(engine, &Engine::applicationError, [&isError, this](QJSValue){
        isError = true;
    });

    QObject* livecvStub = new QObject;
    livecvStub->setProperty("engine", QVariant::fromValue(engine));

    engine->engine()->rootContext()->setContextProperty("engine", engine);
    engine->engine()->rootContext()->setContextProperty("livecv", livecvStub);
    QObject* obj = engine->createObject(
        "import QtQuick 2.3\n import cv 1.0\n "
        "Item{\n"
            "id: root;\n"
            "ErrorHandler{\n"
                "onError:   skip(e);\n"
                "onWarning: skip(e);\n"
            "}\n"
            "EngineTestStub{\n"
                "Component.onCompleted: {\n"
                    "throwJsWarning();"
                    "throwJsError();"
                "}\n"
            "}\n"
        "}\n",
        0,
        QUrl::fromLocalFile("enginetest.qml")
    );

    QVERIFY(obj != 0);

    QCoreApplication::processEvents();

    QVERIFY(isWarning);
    QVERIFY(isError);
}

void EngineTest::engineJsThrownErrorHandlerTest(){

}
