#ifndef MLNODETEST_H
#define MLNODETEST_H

#include <QObject>
#include "testrunner.h"
#include "live/mlnode.h"

class MLNodeTest : public QObject{

    Q_OBJECT
    Q_TEST_RUNNER_SUITE

public:
    MLNodeTest(QObject* parent = 0);
    ~MLNodeTest(){}

private slots:
    void initTestCase();
    void constructorTest();
    void constructorFromInitializerListTest();
    void assignmentTest();
    void accessOperatorTest();
    void base64ToBytesTest();
    void iteratorTest();
    void constIteratorTest();
    void reverseIteratorTest();
    void reverseConstIteratorTest();

};

#endif // MLNODETEST_H
