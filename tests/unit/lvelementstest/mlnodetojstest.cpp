#include "mlnodetojstest.h"
#include "live/elements/value.h"
#include "live/elements/object.h"
#include "live/elements/mlnodetojs.h"
#include "live/elements/engine.h"

Q_TEST_RUNNER_REGISTER(MLNodeToJsTest);

using namespace lv;
using namespace lv::el;


MLNodeToJsTest::MLNodeToJsTest(QObject *parent)
    : QObject(parent)
{

}

void MLNodeToJsTest::initTestCase()
{

}

void MLNodeToJsTest::jsConvertTest(){

    Engine* engine = new Engine();
    engine->scope([&engine](){

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

        LocalValue jv(engine);
        ml::toJs(n, jv, engine);

        QVERIFY(jv.isObject());

        Object o = jv.toObject(engine);
        LocalObject lo(o);

        QVERIFY(lo.get(engine, "object").isObject());
        Object oo = lo.get(engine, "object").toObject(engine);
        LocalObject loo(oo);

        QVERIFY(loo.get(engine, "string").isString());
        QVERIFY(loo.get(engine, "string").toStdString(engine) == "value1");
        QVERIFY(loo.get(engine, "key2").isInt());
        QVERIFY(loo.get(engine, "key2").toInt32(engine) == 100);

        LocalValue jvarray = lo.get(engine, "array");
        QVERIFY(jvarray.isObject());
        Object oa = jvarray.toObject(engine);
        LocalObject loa(oa);

        int oaLen = loa.get(engine, "length").toInt32(engine);
        QVERIFY(oaLen == 3);

        QVERIFY(loa.get(0).isInt());
        QVERIFY(loa.get(0).toInt32(engine) == 100);
        QVERIFY(loa.get(1).isString());
        QVERIFY(loa.get(1).toStdString(engine) == "200");
        QVERIFY(loa.get(2).isBool());
        QVERIFY(loa.get(2).toBool(engine) == false);

        QVERIFY(lo.get(engine, "bool").isBool());
        QVERIFY(lo.get(engine, "bool").toBool(engine) == true);
        QVERIFY(lo.get(engine, "int").isInt());
        QVERIFY(lo.get(engine, "int").toInt32(engine) == 100);
        QVERIFY(lo.get(engine, "float").isNumber());
        QVERIFY(lo.get(engine, "float").toNumber(engine) == 100.1);
        QVERIFY(lo.get(engine, "null").isNull());

        MLNode rt;
        ml::fromJs(jv, rt, engine);

        QCOMPARE(rt.type(), MLNode::Type::Object);
        QCOMPARE(rt["object"].type(), MLNode::Type::Object);
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

    });

    delete engine;

}
