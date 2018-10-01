#ifndef LVCOMPILETEST_H
#define LVCOMPILETEST_H

#include <QObject>
#include "testrunner.h"

class LvCompileTest: public QObject{

    Q_OBJECT
    Q_TEST_RUNNER_SUITE

public:
    LvCompileTest(QObject* parent = 0);
    ~LvCompileTest(){}

private slots:
    void initTestCase();
    void test1Js();
    void test2Js();
    void test3Js();
    void test4Js();

};

#endif // LVCOMPILETEST_H
