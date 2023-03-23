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

    void test1Js();
    void test2Js();
    void test3Js();
    void test4Js();

    void test1Lv();
    void test2Lv();
    void test3Lv();
    void test4Lv();
    void test5Lv();

    void compileModule();
    void compilePackage();

private slots:
    void initTestCase();

    void compilePackageWithRelease();

private:
    std::string scriptPath(const std::string& scriptName);
    std::string testPath();
};

#endif // LVCOMPILETEST_H
