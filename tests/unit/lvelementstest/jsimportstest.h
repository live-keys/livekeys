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
    void importPluginThatImportsPlugin();
    void samePathSingletonTest();
    void importPluginWithSamePathImportTest();
    void packageImportTest();
    void importAsTest();

    void invalidExportTypeAsObjectTest();
    void invalidExportTypeAsStringTest();
    void invalidExportKeyTest();
    void indirectInvalidExportTypeAsObjectTest();

    void moduleFileDependencyCycleTest();
    void pluginDependencyCycleTest();
    void packageDependencyCycleTest();
};

#endif // JSIMPORTSTEST_H
