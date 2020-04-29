#ifndef MEMORYTEST_H
#define MEMORYTEST_H

#include <QObject>
#include "testrunner.h"

class MemoryTest : public QObject{

    Q_OBJECT
    Q_TEST_RUNNER_SUITE

public:
    MemoryTest(QObject *parent = nullptr);
    ~MemoryTest();


private slots:
    void initTestCase();

    void testNoStorage();
    void testStorage();
    void testMultipleStorage();
    void testStorageExceeded();

    void benchmarkSimpleAllocation();
    void benchmarkMemoryAllocation();
    void benchmarkMemoryRecycling();
};

#endif // MEMORYTEST_H
