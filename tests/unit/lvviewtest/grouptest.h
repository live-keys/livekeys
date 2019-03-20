#ifndef GROUPTEST_H
#define GROUPTEST_H

#include <QObject>
#include "testrunner.h"

class GroupTest : public QObject{

    Q_OBJECT
    Q_TEST_RUNNER_SUITE

public:
    GroupTest(QObject* parent = nullptr);
    ~GroupTest(){}

private slots:
    void initTestCase();
    void serializationTest();
    void propertyCreationTestCase();
    void nestedSerializationTest();
};

#endif // GROUPTEST_H
