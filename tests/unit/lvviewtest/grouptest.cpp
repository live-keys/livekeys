#include "grouptest.h"
#include "live/group.h"
#include "live/viewengine.h"
#include "live/mlnode.h"

Q_TEST_RUNNER_REGISTER(GroupTest);

GroupTest::GroupTest(QObject *parent)
    : QObject(parent)
{
}

void GroupTest::initTestCase(){
    qmlRegisterType<lv::Group>("base", 1, 0, "Group");
}

void GroupTest::propertyCreationTestCase(){
    QQmlEngine* engine  = new QQmlEngine;

    lv::Group* gr = lv::Group::createWithProperties(engine, {
        {"var1", "int"},
        {"var2", "double"}
    });

    QVariant var1 = gr->property("var1");
    QVariant var2 = gr->property("var2");
    QVERIFY(var1.isValid());
    QVERIFY(var2.isValid());
    QVERIFY(gr->metaObject()->propertyCount() == 3);

    delete gr;
    delete engine;
}

void GroupTest::serializationTest(){
    QQmlEngine* engine = new QQmlEngine;
    lv::ViewEngine* view = new lv::ViewEngine(engine, nullptr);
    lv::ViewEngine::initializeBaseTypes(view);

    lv::Group* gr = lv::Group::createWithProperties(engine, {
        {"var1", "int"},
        {"var2", "double"}
    });
    gr->setProperty("var1", 100);
    gr->setProperty("var2", -100);

    lv::MLNode result;

    view->typeInfo("lv::Group")->serialize(view, gr, result);

    QVERIFY(result["__type"].asString() == "lv::Group");
    QVERIFY(result["var1"].asInt() == 100);
    QVERIFY(result["var2"].asFloat() == -100);


    lv::Group* roundTrip = qobject_cast<lv::Group*>(view->typeInfo("lv::Group")->deserialize(view, result));

    QVERIFY(roundTrip->metaObject()->propertyCount() == 3);
    QVERIFY(roundTrip->property("var1") == 100);
    QVERIFY(roundTrip->property("var2") == -100);

    delete gr;
    delete roundTrip;
    delete view;
}

void GroupTest::nestedSerializationTest(){
    QQmlEngine* engine = new QQmlEngine;
    lv::ViewEngine* view = new lv::ViewEngine(engine, nullptr);
    lv::ViewEngine::initializeBaseTypes(view);

    lv::Group* nestedgr = lv::Group::createWithProperties(engine, {
        {"var3", "string"}
    });
    nestedgr->setProperty("var3", "value");

    lv::Group* gr = lv::Group::createWithProperties(engine, {
        {"var1", "int"},
        {"var2", "double"},
        {"nested", "Group"}
    });
    gr->setProperty("var1", 100);
    gr->setProperty("var2", -100);
    gr->setProperty("nested", QVariant::fromValue(nestedgr));

    lv::MLNode result;

    view->typeInfo("lv::Group")->serialize(view, gr, result);

    QVERIFY(result["__type"].asString() == "lv::Group");
    QVERIFY(result["var1"].asInt() == 100);
    QVERIFY(result["var2"].asFloat() == -100);
    QVERIFY(result["nested"]["__type"].asString() == "lv::Group");
    QVERIFY(result["nested"]["var3"].asString() == "value");

    lv::Group* roundTrip = qobject_cast<lv::Group*>(view->typeInfo("lv::Group")->deserialize(view, result));

    QVERIFY(roundTrip->metaObject()->propertyCount() == 4);
    QVERIFY(roundTrip->property("var1") == 100);
    QVERIFY(roundTrip->property("var2") == -100);
    QVERIFY(roundTrip->property("nested").value<QObject*>()->metaObject()->inherits(&lv::Group::staticMetaObject));
    QVERIFY(roundTrip->property("nested").value<QObject*>()->property("var3").toString() == "value");

    delete gr;
    delete roundTrip;
    delete view;
}
