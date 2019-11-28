#ifndef LVCOMPILETEST_H
#define LVCOMPILETEST_H

#include <QObject>
#include "testrunner.h"

class LvCompileTest: public QObject{

    Q_OBJECT
    Q_TEST_RUNNER_SUITE

public:
    LvCompileTest(QObject* parent = nullptr);
    ~LvCompileTest(){}

private slots:
    void initTestCase();

    void test1Js();
    void test2Js();
    void test3Js();
    void test4Js();

    void test1Lv();
    void test2Lv();
    void test3Lv();
    void test4Lv();

};

#endif // LVCOMPILETEST_H
