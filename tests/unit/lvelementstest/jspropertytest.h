#ifndef JSPROPERTYTEST_H
#define JSPROPERTYTEST_H

#include <QObject>
#include "testrunner.h"

class JsPropertyTest : public QObject{

    Q_OBJECT
    Q_TEST_RUNNER_SUITE

public:
    JsPropertyTest(QObject* parent = 0);
    ~JsPropertyTest(){}

private slots:
    void initTestCase();

    void readOnlyProperty();
    void propertyTypes();
    void defaultProperty();
    void propertyWithExpression();
    void propertyInheritance();

    void dynamicReadOnlyProperty();
    void dynamicProperty();
    void dynamicPropertyEmptyEvent();
    void dynamicPropertyWithEvent();
    void dynamicPropertyInvalid();
    void dynamicDefaultPropertyExists();
    void dynamicDefaultProperty();
    void jsDynamicPropertyWithAssignedExpression();
    void jsDynamicPropertyWithAddedExpression();

};

#endif // JSPROPERTYTEST_H
