#ifndef JSLISTTEST_H
#define JSLISTTEST_H

#include <QObject>
#include "testrunner.h"


class JsListTest : public QObject{

    Q_OBJECT
    Q_TEST_RUNNER_SUITE

public:
    explicit JsListTest(QObject* parent = 0);
    ~JsListTest(){}

private slots:
    void initTestCase();
    void testFunctions();
    void testEvents();
};


#endif // JSLISTTEST_H
