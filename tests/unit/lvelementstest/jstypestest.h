#ifndef JSTYPESTEST_H
#define JSTYPESTEST_H

#include <QObject>
#include "testrunner.h"

class JsTypesTest : public QObject{

    Q_OBJECT
    Q_TEST_RUNNER_SUITE

public:
    explicit JsTypesTest(QObject *parent = nullptr);
    ~JsTypesTest(){}

private slots:
    void initTestCase();
    void pointTypeTest();
    void sizeTypeTest();
    void rectangleTypeTest();
    void dateTypeTest();

};

#endif // JSTYPESTEST_H
