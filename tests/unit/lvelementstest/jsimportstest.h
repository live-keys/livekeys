#ifndef JSIMPORTSTEST_H
#define JSIMPORTSTEST_H

#include <QObject>
#include "testrunner.h"

class JsImportsTest : public QObject{

    Q_OBJECT
    Q_TEST_RUNNER_SUITE

public:
    explicit JsImportsTest(QObject *parent = nullptr);
    ~JsImportsTest(){}

private slots:
    void initTestCase();

    void singlePluginImportTest();
    void samePathImportTest();
    void importPluginWithSamePathImportTest();
    void importPluginThatImportsPlugin();
    void samePathSingletonTest();

    void invalidExportTypeAsObjectTest();
    void invalidExportTypeAsStringTest();
    void invalidExportKeyTest();
    void indirectInvalidExportTypeAsObjectTest();

    void moduleFileDependencyCycleTest();
    void pluginDependencyCycleTest();
    void packageImportTest();
    void packageDependencyCycleTest();
    void importAsTest();
};

#endif // JSIMPORTSTEST_H
