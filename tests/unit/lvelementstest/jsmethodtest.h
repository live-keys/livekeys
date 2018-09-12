#ifndef JSMETHODTEST_H
#define JSMETHODTEST_H

#include <QObject>
#include "testrunner.h"

class JsMethodTest : public QObject{

    Q_OBJECT
    Q_TEST_RUNNER_SUITE

public:
    JsMethodTest(QObject* parent = nullptr);
    ~JsMethodTest();

private slots:
    void initTestCase();
    void typesTest();

};

#endif // JSMETHODTEST_H
