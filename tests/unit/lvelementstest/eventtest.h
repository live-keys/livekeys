#ifndef LVEVENTTEST_H
#define LVEVENTTEST_H

#include <QObject>
#include "testrunner.h"

class EventTest: public QObject{

    Q_OBJECT
    Q_TEST_RUNNER_SUITE

public:
    EventTest(QObject* parent = 0);
    ~EventTest(){}

private slots:
    void initTestCase();
    void eventGenerationTest();
    void simpleEventEmissionTest();
    void eventEmissionThroughLambdaTest();
    void removeEventFromWithinTest();
    void removeAllEventsFromWithinTest();

};

#endif // LVEVENTTEST_H
