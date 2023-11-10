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

    void singlePluginImportTest();
    void samePathImportTest();
    void importPluginWithSamePathImportTest();
    void importPluginThatImportsPlugin();
    void samePathSingletonTest();
    void sameModuleDifferentNamespacesTest();
    void moduleFileDependencyCycleTest();
    void pluginDependencyCycleTest();
    void packageImportTest();
    void packageDependencyCycleTest();
    void importAsTest();
    void importMetaTest();
    void dashPackageImportTest();
    void rootPackageImportTest();
    void jsImportTest();
    void componentMetaTest();
    void moduleWildcardFilesTest();
    void optionalModuleDefinitionFile();

private:
    std::string scriptPath(const std::string& scriptName);
    std::string testPath();

};

#endif // LVIMPORTSTEST_H
