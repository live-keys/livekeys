#include "mlnodetojsontest.h"
#include <QJsonValue>
#include <QJsonObject>
#include <QJsonArray>

Q_TEST_RUNNER_REGISTER(MLNodeToJsonTest);

using namespace lcv;

MLNodeToJsonTest::MLNodeToJsonTest(QObject *parent)
    : QObject(parent){
}

MLNodeToJsonTest::~MLNodeToJsonTest(){
}

void MLNodeToJsonTest::initTestCase(){
}

void MLNodeToJsonTest::jsonObjectTest(){
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

    QJsonValue jv;
    ml::toJson(n, jv);

    QVERIFY(jv.isObject());
    QCOMPARE(jv.toObject().size(), 6);

    QVERIFY(jv.toObject()["object"].isObject());
    QVERIFY(jv.toObject()["object"].toObject()["string"].isString());
    QCOMPARE(jv.toObject()["object"].toObject()["string"].toString(), QString("value1"));
    QVERIFY(jv.toObject()["object"].toObject()["key2"].isDouble());
    QCOMPARE(jv.toObject()["object"].toObject()["key2"].toInt(), 100);

    QVERIFY(jv.toObject()["array"].isArray());
    QCOMPARE(jv.toObject()["array"].toArray().size(), 3);
    QVERIFY(jv.toObject()["array"].toArray()[0].isDouble());
    QCOMPARE(jv.toObject()["array"].toArray()[0].toInt(), 100);
    QVERIFY(jv.toObject()["array"].toArray()[1].isString());
    QCOMPARE(jv.toObject()["array"].toArray()[1].toString(), QString("200"));
    QVERIFY(jv.toObject()["array"].toArray()[2].isBool());
    QCOMPARE(jv.toObject()["array"].toArray()[2].toBool(), false);

    QVERIFY(jv.toObject()["bool"].isBool());
    QCOMPARE(jv.toObject()["bool"].toBool(), true);
    QVERIFY(jv.toObject()["int"].isDouble());
    QCOMPARE(jv.toObject()["int"].toInt(), 100);
    QVERIFY(jv.toObject()["float"].isDouble());
    QCOMPARE(jv.toObject()["float"].toDouble(), 100.1);
    QVERIFY(jv.toObject()["null"].isNull());

    MLNode rt;
    ml::fromJson(jv, rt);

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

void MLNodeToJsonTest::jsonDataTest(){
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

    QByteArray serialized;
    ml::toJson(n, serialized);

    MLNode rt;
    ml::fromJson(serialized, rt);


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
