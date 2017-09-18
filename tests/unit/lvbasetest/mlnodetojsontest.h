#ifndef MLNODETOJSONTEST_H
#define MLNODETOJSONTEST_H

#include <QObject>
#include "testrunner.h"
#include "live/mlnode.h"
#include "live/mlnodetojson.h"

class MLNodeToJsonTest : public QObject{

    Q_OBJECT
    Q_TEST_RUNNER_SUITE

public:
    explicit MLNodeToJsonTest(QObject *parent = 0);
    ~MLNodeToJsonTest();

private slots:
    void initTestCase();
    void jsonObjectTest();
    void jsonDataTest();

};

#endif // MLNODETOJSONTEST_H
