#ifndef JSMEMORYTEST_H
#define JSMEMORYTEST_H

#include <QObject>
#include "testrunner.h"

class JsMemoryTest : public QObject{

    Q_OBJECT
    Q_TEST_RUNNER_SUITE

public:
    explicit JsMemoryTest(QObject *parent = nullptr);
    ~JsMemoryTest(){}

private slots:
    void initTestCase();
    void singleParentTest();
    void multiParentTest();
    void multiLevelTest();
    void jsAllocationTest();

};

#endif // JSMEMORYTEST_H
