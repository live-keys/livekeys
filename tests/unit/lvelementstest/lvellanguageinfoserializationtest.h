#ifndef LVELLANGUAGEINFOSERIALIZATIONTEST_H
#define LVELLANGUAGEINFOSERIALIZATIONTEST_H

#include <QObject>
#include "testrunner.h"

class LvElLanguageInfoSerializationTest : public QObject
{
    Q_OBJECT
    Q_TEST_RUNNER_SUITE

public:
    explicit LvElLanguageInfoSerializationTest(QObject *parent = nullptr);
    ~LvElLanguageInfoSerializationTest();



private slots:
    void initTestCase();
    void enumInfoToMLNodeTest();
    void fromMLNodeToEnumInfoTest();
    void functionInfoToMLNodeTest();
    void fromMLNodeToFunctionInfoTest();
    void propertyInfoToMLNodeTest();
    void fromMLNodeToPropertyInfoTest();
    void importInfoToMLNodeTest();
    void fromMLNodeToImportInfoNoAliasTest();
    void fromMLNodeToImportInfoTest();
    void typeInfoToMLNodeTest();
    void fromMLNodeToTypeInfoTest();
    void inheritanceInfoToMLNodeTest();
    void fromMLNodeToInheritanceInfoTest();
    void documentInfoToMLNodeTest();
    void fromMLNodeToDocumentInfoTest();
    void moduleInfoToMLNodeTest();
    void fromMLNodeToModuleInfoTest();
};

#endif // LVELLANGUAGEINFOSERIALIZATIONTEST_H
