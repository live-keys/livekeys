#ifndef MLNODETEST_H
#define MLNODETEST_H

#include <QObject>
#include "testrunner.h"

class MLNodeTest : public QObject{

    Q_OBJECT
    Q_TEST_RUNNER_SUITE

public:
    MLNodeTest(QObject* parent = 0);
    ~MLNodeTest(){}

private slots:
    void initTestCase();

};

#endif // MLNODETEST_H
