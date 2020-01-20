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
    void enumInfoToJsonTest();
    void fromJsonToEnumInfoTest();
    void functionInfoToJsonTest();
    void fromJsonToFunctionInfoTest();
    void propertyInfoToJsonTest();
    void fromJsonToPropertyInfoTest();
    void importInfoToJsonTest();
    void fromJsonToImportInfoNoAliasTest();
    void fromJsonToImportInfoTest();
    void typeInfoToJsonTest();
    void fromJsonToTypeInfoTest();
    void inheritanceInfoToJsonTest();
    void fromJsonToInheritanceInfoTest();
    void documentInfoToJsonTest();
    void fromJsonToDocumentInfoTest();
    void moduleInfoToJsonTest();
    void fromJsonToModuleInfoTest();
};

#endif // LVELLANGUAGEINFOSERIALIZATIONTEST_H
