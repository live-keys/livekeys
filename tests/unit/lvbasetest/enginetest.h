#ifndef ENGINETEST_H
#define ENGINETEST_H

#include <QObject>
#include "testrunner.h"

class EngineTest : public QObject{

    Q_OBJECT
    Q_TEST_RUNNER_SUITE

public:
    EngineTest(QObject* parent = 0);
    ~EngineTest(){}

private slots:
    void initTestCase();
    void cppExceptionInObbjectTest();
    void engineExceptionTest();
    void engineWarningTest();

    void engineObjectExceptionTest();
    void engineInternalWarningTest();

    void engineErrorHandlerTest();
    void engineErrorHandlerSkipTest();

    //TODO:
    void engineJsThrownErrorHandlerTest();

};

#endif // ENGINETEST_H
