#ifndef JSERRORHANDLINGTEST_H
#define JSERRORHANDLINGTEST_H

#include <QObject>
#include "testrunner.h"

class JsErrorHandlingTest : public QObject{

    Q_OBJECT
    Q_TEST_RUNNER_SUITE

public:
    JsErrorHandlingTest(QObject* parent = nullptr);
    ~JsErrorHandlingTest(){}

private slots:
    void initTestCase();
    void testException();
    void testExceptionWithLink();
    void testExceptionFromFunction();
    void testExceptionFromMethod();
    void testExceptionFromProperty();
    void testExceptionPropagation();
    void testExceptionPropagationFromJs();
    void testExceptionRethrowPropagation();
};

#endif // JSERRORHANDLINGTEST_H
