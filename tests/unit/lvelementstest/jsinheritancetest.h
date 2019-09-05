#ifndef JSINHERITANCETEST_H
#define JSINHERITANCETEST_H

#include <QObject>
#include "testrunner.h"

class JsInheritanceTest : public QObject{

    Q_OBJECT
    Q_TEST_RUNNER_SUITE

public:
    JsInheritanceTest(QObject *parent = nullptr);

private slots:
    void initTestCase();
    void directElementTest();
    void directInheritanceTest();
    void objectPosingAsElementTest();
    void stringPosingAsElementTest();
    void indirectInheritanceTest();
    void threeLevelInheritanceTest();
    void indirectCppInheritanceTest();
};

#endif // JSINHERITANCETEST_H
