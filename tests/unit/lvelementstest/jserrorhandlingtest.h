#ifndef JSERRORHANDLINGTEST_H
#define JSERRORHANDLINGTEST_H

#include <QObject>
#include "testrunner.h"

class JsErrorHandlingTest : public QObject{

    Q_OBJECT
    Q_TEST_RUNNER_SUITE

public:
    JsErrorHandlingTest(QObject* parent = 0);
    ~JsErrorHandlingTest(){}

private slots:
    void initTestCase();

};

#endif // JSERRORHANDLINGTEST_H
