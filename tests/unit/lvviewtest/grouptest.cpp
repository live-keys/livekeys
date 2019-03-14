#include "grouptest.h"
#include "live/group.h"
#include "live/viewengine.h"

Q_TEST_RUNNER_REGISTER(GroupTest);

GroupTest::GroupTest(QObject *parent)
    : QObject(parent)
{
}

void GroupTest::initTestCase(){
}

void GroupTest::propertyCreationTestCase(){
    qmlRegisterType<lv::Group>("base", 1, 0, "Group");
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
}
