#ifndef LVPARSETEST_H
#define LVPARSETEST_H

#include <QObject>
#include "testrunner.h"
#include "live/fileio.h"

class LvParseTest : public QObject{

    Q_OBJECT
    Q_TEST_RUNNER_SUITE

public:
    explicit LvParseTest(QObject *parent = 0);
    ~LvParseTest(){}

private slots:
    void initTestCase();

    void constructorParseTest();
    void functionParseTest();
    void nestingAndShortIdParseTest();
    void scopeParseTest();
    void propertyNewExpressionParseTest();
    void propertyDeclarationParseTest();
    void propertyExpressionsParseTest();
    void taggedStringParseTest();
    void instanceParseTest();
    void namespaceInheritance();
    void testComplexPropertyAssignment();
    void testSimplePropertyAssignment();
    void nestedLanguageScannerTest();
    void testScenarioTest();
    void nestedFunctionAssignment();
    void arrowFunctionElement();
    void todoListItemSmall();
    void todoList();
    void todoListItem();
    void todoForm();
    void todoApp();
    void complexTernaryOperator();
    void doubleNestedElement();
    void idObjectInJsScope();
    void nestedObjectsInJsScope();
    void sameIdInBothScopes();
    void namespaceImportTest();
    void multiNamespaceImportTest();
    void customBaseComponentTest();
    void defaultChildrenTest();
    void bindShouldNotBeImportedTest();
    void initializeElementTest();
    void customConstructorArgumentsTest();
    void unassignedPropertyTest();
    void staticPropertyTest();
    void functionAnnotationTest();
    void jsImportsTest();
    void trippleTagTest();
    void componentImportTest();
    void getterSetterTest();
    void jsSectionsTest();
    void eventsMethodsAndListenersTest();
    void propertyAssignmentInBindingTest();
    void instanceIdsTest();
    void bindingChainTest();
    void creationInMethodTest();
    void propertyEqualsVsBinding();
    void propertyNotation();

private:
    void parseTestTemplate(std::string name);

    lv::FileIO*  m_fileIO;
    std::string  m_scriptPath;
};

#endif // LVPARSETEST_H
