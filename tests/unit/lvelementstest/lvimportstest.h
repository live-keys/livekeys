#ifndef LVIMPORTSTEST_H
#define LVIMPORTSTEST_H

#include <QObject>
#include "testrunner.h"

class LvImportsTest : public QObject
{
    Q_OBJECT
    Q_TEST_RUNNER_SUITE

public:
    explicit LvImportsTest(QObject *parent = nullptr);
private slots:
    void initTestCase();

    void singlePluginImportTest();
    void samePathImportTest();
    void importPluginWithSamePathImportTest();
    void importPluginThatImportsPlugin();
    void samePathSingletonTest();

    void moduleFileDependencyCycleTest();
    void pluginDependencyCycleTest();
    void packageImportTest();
    void importAsTest();

};

#endif // LVIMPORTSTEST_H
