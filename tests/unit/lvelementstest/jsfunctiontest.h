#ifndef JSFUNCTIONTEST_H
#define JSFUNCTIONTEST_H

#include <QObject>
#include "testrunner.h"

class JsFunctionTest : public QObject{

    Q_OBJECT
    Q_TEST_RUNNER_SUITE

public:
    JsFunctionTest(QObject *parent = nullptr);
    ~JsFunctionTest();

private slots:
    void initTestCase();
    void typesTest();
};

#endif // JSFUNCTIONTEST_H
