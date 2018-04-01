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

#include "mlnodetojstest.h"
#include <QJSValue>
#include <QJSEngine>

Q_TEST_RUNNER_REGISTER(MLNodeToJsTest);

using namespace lv;

MLNodeToJsTest::MLNodeToJsTest(QObject *parent)
    : QObject(parent)
{
}

MLNodeToJsTest::~MLNodeToJsTest(){
}

void MLNodeToJsTest::initTestCase(){
}

void MLNodeToJsTest::jsConvertTest(){
    MLNode n = {
        {"object", {
             {"string", "value1"},
             {"key2", 100}
        }},
        {"array", { 100, "200", false}},
        {"bool", true},
        {"int", 100},
        {"float", 100.1},
        {"null", nullptr}
    };

    QJSEngine engine;

    QJSValue jv;
    ml::toJs(n, jv, &engine);

    QVERIFY(jv.isObject());
    QVERIFY(jv.property("object").isObject());
    QVERIFY(jv.property("object").property("string").isString());
    QCOMPARE(jv.property("object").property("string").toString(), QString("value1"));
    QVERIFY(jv.property("object").property("key2").isNumber());
    QCOMPARE(jv.property("object").property("key2").toInt(), 100);

    QVERIFY(jv.property("array").isArray());
    QVERIFY(jv.property("array").property(0).isNumber());
    QCOMPARE(jv.property("array").property(0).toInt(), 100);
    QVERIFY(jv.property("array").property(1).isString());
    QCOMPARE(jv.property("array").property(1).toString(), QString("200"));
    QVERIFY(jv.property("array").property(2).isBool());
    QCOMPARE(jv.property("array").property(2).toBool(), false);

    QVERIFY(jv.property("bool").isBool());
    QCOMPARE(jv.property("bool").toBool(), true);
    QVERIFY(jv.property("int").isNumber());
    QCOMPARE(jv.property("int").toInt(), 100);
    QVERIFY(jv.property("float").isNumber());
    QCOMPARE(jv.property("float").toNumber(), 100.1);
    QVERIFY(jv.property("null").isNull());

    MLNode rt;
    ml::fromJs(jv, rt);

    QCOMPARE(rt.type(), MLNode::Type::Object);
    QCOMPARE(rt["object"].type(), MLNode::Type::Object);
    QCOMPARE(rt["object"].size(), 2);
    QCOMPARE(rt["object"]["string"].asString(), MLNode::StringType("value1"));
    QCOMPARE(rt["object"]["key2"].asInt(), 100);

    QCOMPARE(rt["array"].size(), 3);
    QCOMPARE(rt["array"][0].asInt(), 100);
    QCOMPARE(rt["array"][1].asString(), MLNode::StringType("200"));
    QCOMPARE(rt["array"][2].asBool(), false);

    QCOMPARE(rt["bool"].asBool(), true);
    QCOMPARE(rt["int"].asInt(), 100);
    QCOMPARE(rt["float"].asFloat(), 100.1);
    QVERIFY(rt["null"].isNull());
}
