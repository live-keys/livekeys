#ifndef JSEVENTTEST_H
#define JSEVENTTEST_H

#include <QObject>
#include "testrunner.h"

class JsEventTest: public QObject{

    Q_OBJECT
    Q_TEST_RUNNER_SUITE

public:
    JsEventTest(QObject* parent = 0);
    ~JsEventTest(){}

private slots:
    void initTestCase();

    void callEventTest();
    void registerListenerTest();
    void registerListenerThroughMetaTest();
    void triggerEventThroughMetaTest();
    void eventOnProperty();
    void eventOnCustomProperty();
    void addEventJsCallInJsTest();
    void addEventListenInJsTest();

    void eventTypesTest();
    void eventInheritanceTest();
};

#endif // JSEVENTTEST_H
