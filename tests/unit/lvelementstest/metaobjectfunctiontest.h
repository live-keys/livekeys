#ifndef METAOBJECTFUNCTIONTEST_H
#define METAOBJECTFUNCTIONTEST_H

#include <QObject>
#include "testrunner.h"

class MetaObjectFunctionTest : public QObject{

    Q_OBJECT
    Q_TEST_RUNNER_SUITE

public:
    MetaObjectFunctionTest(QObject* parent = 0);

private slots:
    void initTestCase();
    void simpleFunctionTest();
    void optionalArgumentsTypesTest();

};

#endif // METAOBJECTFUNCTIONTEST_H
