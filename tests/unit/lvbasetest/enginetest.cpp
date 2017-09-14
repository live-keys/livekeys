#include "enginetest.h"
#include "engineteststub.h"
#include "live/exception.h"
#include "live/engine.h"
#include <QQmlEngine>

Q_TEST_RUNNER_REGISTER(EngineTest);

using namespace lcv;

EngineTest::EngineTest(QObject *parent)
    : QObject(parent)
{
}

void EngineTest::initTestCase(){
    qmlRegisterType<EngineTestStub>("cv", 1, 0, "EngineTestStub");
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

    bool isEWarning = false;

    QObject::connect(&engine, &Engine::applicationWarning, [&isEWarning, this](QJSValue error){
        QCOMPARE(error.property("message").toString(), QString("JSTest"));
        QCOMPARE(error.property("fileName").toString(), QString("enginetest.cpp"));
        QCOMPARE(error.property("lineNumber").toInt(), 100);
        QCOMPARE(error.property("object").toQObject(), this);
        isEWarning = true;
    });

    engine.throwWarning("JSTest", this, "enginetest.cpp", 100);
    QCoreApplication::processEvents();

    QVERIFY(isEWarning);
}
