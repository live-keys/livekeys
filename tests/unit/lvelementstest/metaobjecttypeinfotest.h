#ifndef METAOBJECTTYPEINFOTEST_H
#define METAOBJECTTYPEINFOTEST_H

#include <QObject>
#include "testrunner.h"

class MetaObjectTypeInfoTest : public QObject{

    Q_OBJECT
    Q_TEST_RUNNER_SUITE

public:
    explicit MetaObjectTypeInfoTest(QObject *parent = nullptr);
    ~MetaObjectTypeInfoTest(){}

private slots:
    void initTestCase();

    void elementTest();
    void containerTest();
};

#endif // METAOBJECTTYPEINFOTEST_H
