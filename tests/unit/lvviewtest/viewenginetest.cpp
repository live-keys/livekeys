/****************************************************************************
**
** Copyright (C) 2022 Dinu SV.
**
** This file is part of Livekeys Application.
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

#include "viewenginetest.h"
#include "viewengineteststub.h"
#include "live/errorhandler.h"
#include "live/exception.h"
#include "live/viewengine.h"
#include <QVariant>
#include <QQmlEngine>
#include <QQmlContext>

Q_TEST_RUNNER_REGISTER(ViewEngineTest);

using namespace lv;

ViewEngineTest::ViewEngineTest(QObject *parent)
    : QObject(parent)
{
}

void ViewEngineTest::initTestCase(){
    qmlRegisterType<ViewEngineTestStub>("base", 1, 0, "EngineTestStub");
    qmlRegisterType<lv::ErrorHandler>("base", 1, 0, "ErrorHandler");
}

void ViewEngineTest::cppExceptionInObbjectTest(){
    ViewEngine engine(new QQmlEngine, nullptr);

    QObject* obj = engine.createObject(
        "import QtQuick 2.3\n import base 1.0\n "
        "EngineTestStub{}",
                nullptr,
        QUrl::fromLocalFile("enginetest.qml")
    );

    QVERIFY(obj != nullptr);

    bool isException = false;
    try{
        QMetaObject::invokeMethod(obj, "throwException");
        QCoreApplication::processEvents();
    } catch ( lv::Exception& ){
        isException = true;
    }
    QVERIFY(isException);
}

void ViewEngineTest::engineExceptionTest(){
    ViewEngine engine(new QQmlEngine, nullptr);

    lv::Exception exception = lv::Exception::create<lv::Exception>(
        "JSTest", 1, "enginetest.cpp", 100, "jsExceptionInObjectTest"
    );

    bool isException = false;

    QObject::connect(&engine, &ViewEngine::applicationError, [&isException](QJSValue error){
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

void ViewEngineTest::engineObjectExceptionTest(){
    ViewEngine engine(new QQmlEngine, nullptr);

    bool isException = false;
    QObject::connect(&engine, &ViewEngine::applicationError, [&isException](QJSValue error){
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
        nullptr,
        QUrl::fromLocalFile("enginetest.qml")
    );

    QVERIFY(obj != nullptr);

    QCoreApplication::processEvents();

    QVERIFY(isException);
}

void ViewEngineTest::engineInternalWarningTest(){
    ViewEngine engine(new QQmlEngine, nullptr);

    bool isWarning = false;
    QObject::connect(&engine, &ViewEngine::applicationWarning, [&isWarning](QJSValue error){
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
        nullptr,
        QUrl::fromLocalFile("enginetest.qml")
    );

    QVERIFY(obj != nullptr);

    QCoreApplication::processEvents();

    QVERIFY(isWarning);
}

void ViewEngineTest::engineErrorHandlerTest(){
    ViewEngine* engine = new ViewEngine(new QQmlEngine, nullptr);

    bool isWarning = false;
    bool isError   = false;
    QObject::connect(engine, &ViewEngine::applicationWarning, [&isWarning](QJSValue){
        isWarning = true;
    });
    QObject::connect(engine, &ViewEngine::applicationError, [&isError](QJSValue){
        isError = true;
    });

    QObject livekeysStub;
    livekeysStub.setProperty("engine", QVariant::fromValue(engine));

    engine->engine()->rootContext()->setContextProperty("engine", engine);
    engine->engine()->rootContext()->setContextProperty("lk", &livekeysStub);
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
        nullptr,
        QUrl::fromLocalFile("enginetest.qml")
    );

    QVERIFY(obj != nullptr);

    QCoreApplication::processEvents();

    QVERIFY(!isWarning);
    QVERIFY(!isError);

    QCOMPARE(obj->property("errorMessage").toString(),   QString("JSTest"));
    QCOMPARE(obj->property("warningMessage").toString(), QString("JSTest"));
}

void ViewEngineTest::engineErrorHandlerSkipTest(){
    ViewEngine* engine = new ViewEngine(new QQmlEngine, nullptr);

    bool isWarning = false;
    bool isError   = false;
    QObject::connect(engine, &ViewEngine::applicationWarning, [&isWarning](QJSValue){
        isWarning = true;
    });
    QObject::connect(engine, &ViewEngine::applicationError, [&isError](QJSValue){
        isError = true;
    });

    QObject livekeysStub;
    livekeysStub.setProperty("engine", QVariant::fromValue(engine));

    engine->engine()->rootContext()->setContextProperty("engine", engine);
    engine->engine()->rootContext()->setContextProperty("lk", &livekeysStub);
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
        nullptr,
        QUrl::fromLocalFile("enginetest.qml")
    );

    QVERIFY(obj != nullptr);

    QCoreApplication::processEvents();

    QVERIFY(isWarning);
    QVERIFY(isError);
}

void ViewEngineTest::jsThrownErrorTest(){
    ViewEngine engine(new QQmlEngine, nullptr);

    bool isError = false;
    QObject::connect(&engine, &ViewEngine::applicationError, [&isError](QJSValue error){
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
        nullptr,
        QUrl::fromLocalFile("enginetest.qml")
    );

    QVERIFY(obj != nullptr);

    QCoreApplication::processEvents();

    QVERIFY(isError);
}

void ViewEngineTest::jsThrownErrorHandlerTest(){
    //See how I can extend exception
    ViewEngine* engine = new ViewEngine(new QQmlEngine, nullptr);

    bool isError = false;
    QObject::connect(engine, &ViewEngine::applicationError, [&isError](QJSValue){
        isError = true;
    });

    QObject* livekeysStub = new QObject;
    livekeysStub->setProperty("engine", QVariant::fromValue(engine));

    engine->engine()->rootContext()->setContextProperty("engine", engine);
    engine->engine()->rootContext()->setContextProperty("lk", livekeysStub);
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
        nullptr,
        QUrl::fromLocalFile("enginetest.qml")
    );

    QVERIFY(obj != nullptr);

    QCoreApplication::processEvents();

    QVERIFY(!isError);
    QVERIFY(obj->property("errorMessage").toString().endsWith("JSTest"));

    delete livekeysStub;
}
