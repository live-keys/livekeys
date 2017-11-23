/****************************************************************************
**
** Copyright (C) 2014-2017 Dinu SV.
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

#include "mlnodetest.h"

Q_TEST_RUNNER_REGISTER(MLNodeTest);

using namespace lv;

MLNodeTest::MLNodeTest(QObject *parent)
    : QObject(parent)
{
}

void MLNodeTest::initTestCase(){
}

void MLNodeTest::constructorTest(){
    QCOMPARE(MLNode().type(),                     MLNode::Null);
    QCOMPARE(MLNode("str").type(),                MLNode::String);
    QCOMPARE(MLNode(MLNode::StringType()).type(), MLNode::String);
    QCOMPARE(MLNode(200).type(),                  MLNode::Integer);
    QCOMPARE(MLNode(200.2).type(),                MLNode::Float);
    QCOMPARE(MLNode(true).type(),                 MLNode::Boolean);
    QCOMPARE(MLNode(MLNode::BytesType()).type(),  MLNode::Bytes);
    QCOMPARE(MLNode(MLNode::ObjectType()).type(), MLNode::Object);
    QCOMPARE(MLNode(MLNode::ArrayType()).type(),  MLNode::Array);
}

void MLNodeTest::constructorFromInitializerListTest(){
    MLNode n({100, 200, 300});
    QCOMPARE(n.type(), MLNode::Array);

    MLNode nObject({{"object", "value"}});
    QCOMPARE(nObject.type(), MLNode::Object);
}

void MLNodeTest::assignmentTest(){
    MLNode n = 100;
    QCOMPARE(n.type(), MLNode::Integer);
    n = 100.20;
    QCOMPARE(n.type(), MLNode::Float);

    n = {100, 200, 300};
    QCOMPARE(n.type(), MLNode::Array);

    n = {{"object", "value"}};
    QCOMPARE(n.type(), MLNode::Object);

    unsigned char* data = new unsigned char[10];
    for ( unsigned char i = 0; i < 10; ++i )
        data[i] = i;
    n = MLNode::BytesType(data, 10);
    QCOMPARE(n.type(), MLNode::Bytes);
    delete[] data;
}

void MLNodeTest::accessOperatorTest(){
    MLNode n = 100;
    QVERIFY_EXCEPTION_THROWN(n[100],   InvalidMLTypeException);
    QVERIFY_EXCEPTION_THROWN(n["200"], InvalidMLTypeException);

    n = {100, 200, 300};
    QCOMPARE(n[0].asInt(), 100);
    QCOMPARE(n[1].asInt(), 200);
    QCOMPARE(n[2].asInt(), 300);
    QVERIFY_EXCEPTION_THROWN(n["200"], InvalidMLTypeException);

    n = {{"key", "value"}};
    QVERIFY_EXCEPTION_THROWN(n[100], InvalidMLTypeException);
    QCOMPARE(n["key"].asString(), MLNode::StringType("value"));

    n = {
        {"object", {
             {"key1", "value1"},
             {"key2", 100}
        }},
        {"array", { 100, "200", false}},
        {"bool", true},
        {"int", 100},
        {"float", 100.1}
    };

    QCOMPARE(n["object"]["key1"].asString(), MLNode::StringType("value1"));
    QCOMPARE(n["object"]["key2"].asInt(), 100);
    QCOMPARE(n["array"][0].asInt(), 100);
    QCOMPARE(n["array"][1].asString(), MLNode::StringType("200"));
    QCOMPARE(n["array"][2].asBool(), false);
    QCOMPARE(n["bool"].asBool(), true);
    QCOMPARE(n["int"].asInt(), 100);
    QCOMPARE(n["float"].asFloat(), 100.1);
}

void MLNodeTest::base64ToBytesTest(){
    const char* str = "!@(^$#*(@$!:";
    QByteArray base64 = QByteArray(str).toBase64();

    MLNode n = MLNode::StringType(base64.constData());
    QCOMPARE(n.type(), MLNode::String);

    MLNode::BytesType roundtrip = n.asBytes();
    QCOMPARE(MLNode::StringType(reinterpret_cast<const char*>(roundtrip.data()), roundtrip.size()), MLNode::StringType(str));
}

void MLNodeTest::iteratorTest(){
    MLNode n;
    n = MLNode::ArrayType();
    QCOMPARE(n.begin(), n.end());


    n = {100, 200, 300};
    MLNode::Iterator it = n.begin();
    QCOMPARE(it.value().asInt(), 100);
    QVERIFY(it < n.end());
    ++it;
    QCOMPARE(it->asInt(), 200);
    ++it;
    QCOMPARE(it->asInt(), 300);
    ++it;
    QCOMPARE(it, n.end());

    it = n.begin() + 1;
    QCOMPARE(it->asInt(), 200);
    it += 2;
    QCOMPARE(it, n.end());
    --it;
    QCOMPARE(it->asInt(), 300);
    it -= 2;
    QCOMPARE(it->asInt(), 100);
    QCOMPARE(it, n.begin());


    n = 100;
    it = n.begin();
    QCOMPARE(it.value().asInt(), 100);
    QCOMPARE(it + 1, n.end());

    n = {
        {"a", {
             {"key1", "value1"},
             {"key2", 100}
        }},
        {"b", { 100, "200", false}},
        {"c", true},
        {"d", 100},
        {"e", 100.1}
    };

    it = n.begin();
    QVERIFY_EXCEPTION_THROWN(n.begin() + 3, InvalidMLTypeException);

    QCOMPARE(it.key(), MLNode::StringType("a"));
    QVERIFY_EXCEPTION_THROWN(it += 3, InvalidMLTypeException);
    ++it;
    QCOMPARE(it.key(), MLNode::StringType("b"));
    QCOMPARE(it.value().begin()->asInt(), 100);
    ++it;
    QCOMPARE(it.key(), MLNode::StringType("c"));
    QCOMPARE(it.value().asBool(), true);
    ++it;
    QCOMPARE(it.key(), MLNode::StringType("d"));
    QCOMPARE(it->asInt(), 100);
    --it;
    QCOMPARE(it.key(), MLNode::StringType("c"));
    ++it;
    ++it;
    ++it;
    QCOMPARE(it, n.end());
}

void MLNodeTest::constIteratorTest(){
    MLNode n;

    n = {100, 200, 300};
    MLNode::ConstIterator it = n.begin();
    QCOMPARE(it.value().asInt(), 100);
    QVERIFY(it < n.end());
    ++it;
    QCOMPARE(it->asInt(), 200);
    ++it;
    QCOMPARE(it->asInt(), 300);
    ++it;
    QCOMPARE(it, n.cend());

    it = n.cbegin() + 1;
    QCOMPARE(it->asInt(), 200);
    it += 2;
    QCOMPARE(it, n.cend());
    --it;
    QCOMPARE(it->asInt(), 300);
    it -= 2;
    QCOMPARE(it->asInt(), 100);
    QCOMPARE(it, n.cbegin());


    n = 100;
    it = n.cbegin();
    QCOMPARE(it.value().asInt(), 100);
    QCOMPARE(it + 1, n.cend());

    n = {
        {"a", {
             {"key1", "value1"},
             {"key2", 100}
        }},
        {"b", { 100, "200", false}},
        {"c", true},
        {"d", 100},
        {"e", 100.1}
    };

    it = n.cbegin();
    QVERIFY_EXCEPTION_THROWN(n.begin() + 3, InvalidMLTypeException);

    QCOMPARE(it.key(), MLNode::StringType("a"));
    QVERIFY_EXCEPTION_THROWN(it += 3, InvalidMLTypeException);
    ++it;
    QCOMPARE(it.key(), MLNode::StringType("b"));
    QCOMPARE(it.value().begin()->asInt(), 100);
    ++it;
    QCOMPARE(it.key(), MLNode::StringType("c"));
    QCOMPARE(it.value().asBool(), true);
    ++it;
    QCOMPARE(it.key(), MLNode::StringType("d"));
    QCOMPARE(it->asInt(), 100);
    --it;
    QCOMPARE(it.key(), MLNode::StringType("c"));
    ++it;
    ++it;
    ++it;
    QCOMPARE(it, n.cend());
}


