/****************************************************************************
**
** Copyright (C) 2014-2018 Dinu SV.
** (contact: mail@dinusv.com)
** This file is part of Live CV Application.
**
** GNU Lesser General Public License Usage
** This file may be used under the terms of the GNU Lesser
** General Public License version 3 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPLv3 included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 3 requirements
** will be met: https://www.gnu.org/licenses/lgpl.html.
**
****************************************************************************/

#include "enginetest.h"
#include "engineteststub.h"
#include "live/errorhandler.h"
#include "live/exception.h"
#include "live/viewengine.h"
#include <QVariant>
#include <QQmlEngine>
#include <QQmlContext>

Q_TEST_RUNNER_REGISTER(EngineTest);

using namespace lv;

EngineTest::EngineTest(QObject *parent)
    : QObject(parent)
{
}

void EngineTest::initTestCase(){
    qmlRegisterType<EngineTestStub>("base", 1, 0, "EngineTestStub");
    qmlRegisterType<lv::ErrorHandler>("base", 1, 0, "ErrorHandler");
}

void EngineTest::cppExceptionInObbjectTest(){
    ViewEngine engine(new QQmlEngine);

    QObject* obj = engine.createObject(
        "import QtQuick 2.3\n import base 1.0\n "
        "EngineTestStub{}",
        0,
        QUrl::fromLocalFile("enginetest.qml")
    );

    QVERIFY(obj != 0);

    bool isException = false;
    try{
        QMetaObject::invokeMethod(obj, "throwException");
        QCoreApplication::processEvents();
    } catch ( lv::Exception& ){
        isException = true;
    }
    QVERIFY(isException);
}

void EngineTest::engineExceptionTest(){
    ViewEngine engine(new QQmlEngine);

    lv::Exception exception = lv::Exception::create<lv::Exception>(
        "JSTest", 1, "enginetest.cpp", 100, "jsExceptionInObjectTest"
    );

    bool isException = false;

    QObject::connect(&engine, &ViewEngine::applicationError, [&isException, this](QJSValue error){
        QCOMPARE(error.property("type").toString(), QString("Exception"));
        QCOMPARE(error.property("message").toString(), QString("JSTest"));
        QCOMPARE(error.property("code").toInt(), 1);
        QCOMPARE(error.property("functionName").toString(), QString("jsExceptionInObjectTest"));
        QCOMPARE(error.property("fileName").toString(), QString("enginetest.cpp"));
        QCOMPARE(error.property("lineNumber").toInt(), 100);
        isException = true;
    });

    engine.throwError(&exception, nullptr);
    QCoreApplication::processEvents();

    QVERIFY(isException);
}

void EngineTest::engineWarningTest(){
    ViewEngine engine(new QQmlEngine);

    bool isWarning = false;

    QObject::connect(&engine, &ViewEngine::applicationWarning, [&isWarning, this](QJSValue error){
        QCOMPARE(error.property("message").toString(), QString("JSTest"));
        QCOMPARE(error.property("fileName").toString(), QString("enginetest.cpp"));
        QCOMPARE(error.property("lineNumber").toInt(), 100);
        isWarning = true;
    });

    engine.throwWarning("JSTest", nullptr, "enginetest.cpp", 100);
    QCoreApplication::processEvents();

    QVERIFY(isWarning);
}

void EngineTest::engineObjectExceptionTest(){
    ViewEngine engine(new QQmlEngine);

    bool isException = false;
    QObject::connect(&engine, &ViewEngine::applicationError, [&isException, this](QJSValue error){
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
        "import QtQuick 2.3\n import base 1.0\n "
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
    ViewEngine engine(new QQmlEngine);

    bool isWarning = false;
    QObject::connect(&engine, &ViewEngine::applicationWarning, [&isWarning, this](QJSValue error){
        QCOMPARE(error.property("message").toString(), QString("JSTest"));
        QCOMPARE(error.property("fileName").toString(), QString("enginetest.cpp"));
        QCOMPARE(error.property("lineNumber").toInt(), 100);
        isWarning = true;
    });

    engine.engine()->rootContext()->setContextProperty("engine", &engine);
    QObject* obj = engine.createObject(
        "import QtQuick 2.3\n import base 1.0\n "
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
    ViewEngine* engine = new ViewEngine(new QQmlEngine);

    bool isWarning = false;
    bool isError   = false;
    QObject::connect(engine, &ViewEngine::applicationWarning, [&isWarning, this](QJSValue){
        isWarning = true;
    });
    QObject::connect(engine, &ViewEngine::applicationError, [&isError, this](QJSValue){
        isError = true;
    });

    QObject livecvStub;
    livecvStub.setProperty("engine", QVariant::fromValue(engine));

    engine->engine()->rootContext()->setContextProperty("engine", engine);
    engine->engine()->rootContext()->setContextProperty("livecv", &livecvStub);
    QObject* obj = engine->createObject(
        "import QtQuick 2.3\n import base 1.0\n "
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
    ViewEngine* engine = new ViewEngine(new QQmlEngine);

    bool isWarning = false;
    bool isError   = false;
    QObject::connect(engine, &ViewEngine::applicationWarning, [&isWarning, this](QJSValue){
        isWarning = true;
    });
    QObject::connect(engine, &ViewEngine::applicationError, [&isError, this](QJSValue){
        isError = true;
    });

    QObject livecvStub;
    livecvStub.setProperty("engine", QVariant::fromValue(engine));

    engine->engine()->rootContext()->setContextProperty("engine", engine);
    engine->engine()->rootContext()->setContextProperty("livecv", &livecvStub);
    QObject* obj = engine->createObject(
        "import QtQuick 2.3\n import base 1.0\n "
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

void EngineTest::jsThrownErrorTest(){
    ViewEngine engine(new QQmlEngine);

    bool isError = false;
    QObject::connect(&engine, &ViewEngine::applicationError, [&isError, this](QJSValue error){
        QVERIFY(error.property("message").toString().endsWith("JSTest"));
        QVERIFY(error.property("fileName").toString().endsWith("enginetest.qml"));
        QCOMPARE(error.property("lineNumber").toInt(), 3);
        isError = true;
    });

    engine.engine()->rootContext()->setContextProperty("engine", &engine);
    QObject* obj = engine.createObject(
        "import QtQuick 2.3\n import base 1.0\n "
        "EngineTestStub{"
            "Component.onCompleted: {throw new Error('JSTest');}"
        "}",
        0,
        QUrl::fromLocalFile("enginetest.qml")
    );

    QVERIFY(obj != 0);

    QCoreApplication::processEvents();

    QVERIFY(isError);
}

void EngineTest::jsThrownErrorHandlerTest(){
    //See how I can extend exception
    ViewEngine* engine = new ViewEngine(new QQmlEngine);

    bool isError = false;
    QObject::connect(engine, &ViewEngine::applicationError, [&isError, this](QJSValue){
        isError = true;
    });

    QObject* livecvStub = new QObject;
    livecvStub->setProperty("engine", QVariant::fromValue(engine));

    engine->engine()->rootContext()->setContextProperty("engine", engine);
    engine->engine()->rootContext()->setContextProperty("livecv", livecvStub);
    QObject* obj = engine->createObject(
        "import QtQuick 2.3\n import base 1.0\n "
        "Item{\n"
            "id: root;\n"
            "property string errorMessage: 'empty';\n"
            "ErrorHandler{\n"
                "onError:   root.errorMessage   = e.message;\n"
            "}\n"
            "EngineTestStub{\n"
                "Component.onCompleted: {\n"
                    "throw linkError(new Error('JSTest'), this);"
                "}\n"
            "}\n"
        "}\n",
        0,
        QUrl::fromLocalFile("enginetest.qml")
    );

    QVERIFY(obj != 0);

    QCoreApplication::processEvents();

    QVERIFY(!isError);
    QVERIFY(obj->property("errorMessage").toString().endsWith("JSTest"));

    delete livecvStub;
}
