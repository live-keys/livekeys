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
    QVERIFY_EXCEPTION_THROWN(QMetaObject::invokeMethod(obj, "throwException"), lcv::Exception);
}

void EngineTest::cppExceptionInQmlTest(){
    Engine engine(new QQmlEngine);

    const char* code =
        "import QtQuick 2.3\n import cv 1.0\n "
        "EngineTestStub{"
            "Component.onCompleted: throwException()"
        "}";

    QVERIFY_EXCEPTION_THROWN(engine.createObject(code, 0, QUrl::fromLocalFile("enginetest.qml")), lcv::Exception);
}
