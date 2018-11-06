#ifndef MLNODETOJSTEST_H
#define MLNODETOJSTEST_H

#include <QObject>
#include "testrunner.h"

class MLNodeToJsTest : public QObject{

    Q_OBJECT
    Q_TEST_RUNNER_SUITE

public:
    MLNodeToJsTest(QObject* parent = 0);
    ~MLNodeToJsTest(){}

private slots:
    void initTestCase();
    void jsConvertTest();

};

#endif // MLNODETOJSTEST_H
