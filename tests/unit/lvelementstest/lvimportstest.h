#ifndef LVIMPORTSTEST_H
#define LVIMPORTSTEST_H

#include <QObject>
#include "testrunner.h"

class LvImportsTest : public QObject{

    Q_OBJECT
    Q_TEST_RUNNER_SUITE

public:
    explicit LvImportsTest(QObject *parent = nullptr);

private slots:
    void initTestCase();

    void importPluginThatImportsPlugin();
    void importAsTest();
    void singlePluginImportTest();
    void samePathImportTest();
    void importPluginWithSamePathImportTest();
    void samePathSingletonTest();
    void sameModuleDifferentNamespacesTest();
    void moduleFileDependencyCycleTest();
    void pluginDependencyCycleTest();
    void packageImportTest();
    void packageDependencyCycleTest();
    void importMetaTest();

private:
    std::string scriptPath(const std::string& scriptName);
    std::string testPath();

};

#endif // LVIMPORTSTEST_H
