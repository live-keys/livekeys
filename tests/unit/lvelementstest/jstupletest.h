#ifndef JSTUPLETEST_H
#define JSTUPLETEST_H

#include <QObject>
#include "testrunner.h"

class JsTupleTest : public QObject{

    Q_OBJECT
    Q_TEST_RUNNER_SUITE

public:
    explicit JsTupleTest(QObject *parent = nullptr);
    ~JsTupleTest();

private slots:
    void initTestCase();

    void propertyAdditionTest();

};

#endif // JSTUPLETEST_H
