#include "lvparsetest.h"
#include "live/fileio.h"
#include "live/applicationcontext.h"

#include "live/elements/element.h"
#include "live/elements/metaobject.h"
#include "live/elements/component.h"
#include "live/elements/modulelibrary.h"
#include "live/elements/compiler/languageparser.h"

Q_TEST_RUNNER_REGISTER(LvParseTest);

using namespace lv;
using namespace lv::el;

LvParseTest::LvParseTest(QObject *parent)
    : QObject(parent)
{
}

void LvParseTest::initTestCase(){
    m_fileIO = new FileIO;
    m_scriptPath = Path::parent(lv::ApplicationContext::instance().applicationFilePath()) + "/data";
}

void LvParseTest::constructorParseTest(){
    parseTestTemplate("ParserTest01");
}

void LvParseTest::functionParseTest(){
    parseTestTemplate("ParserTest02");
}

void LvParseTest::nestingAndShortIdParseTest(){
    parseTestTemplate("ParserTest03");
}

void LvParseTest::scopeParseTest(){
    parseTestTemplate("ParserTest04");
}

void LvParseTest::propertyNewExpressionParseTest(){
    parseTestTemplate("ParserTest05");
}

void LvParseTest::propertyDeclarationParseTest(){
    parseTestTemplate("ParserTest06");
}

void LvParseTest::propertyExpressionsParseTest(){
    parseTestTemplate("ParserTest07");
}

void LvParseTest::taggedStringParseTest(){
    parseTestTemplate("ParserTest08");
}

void LvParseTest::instanceParseTest(){
    parseTestTemplate("ParserTest09");
}

void LvParseTest::namespaceInheritance(){
    parseTestTemplate("ParserTest10");
}

void LvParseTest::testComplexPropertyAssignment(){
    parseTestTemplate("ParserTest11");
}

void LvParseTest::testSimplePropertyAssignment(){
    parseTestTemplate("ParserTest12");
}

void LvParseTest::testScenarioTest(){
    parseTestTemplate("ParserTest13");
}

void LvParseTest::nestedLanguageScannerTest(){
    parseTestTemplate("ParserTest14");
}

void LvParseTest::nestedFunctionAssignment(){
    parseTestTemplate("ParserTest15");
}

void LvParseTest::arrowFunctionElement(){
    parseTestTemplate("ParserTest16");
}

void LvParseTest::todoListItemSmall(){
    parseTestTemplate("ParserTest17");
}

void LvParseTest::todoList(){
    parseTestTemplate("ParserTest18");
}

void LvParseTest::todoListItem(){
    parseTestTemplate("ParserTest19");
}

void LvParseTest::todoForm(){
    parseTestTemplate("ParserTest20");
}

void LvParseTest::todoApp(){
    parseTestTemplate("ParserTest21");
}

void LvParseTest::complexTernaryOperator(){
    parseTestTemplate("ParserTest22");
}

void LvParseTest::doubleNestedElement(){
    parseTestTemplate("ParserTest23");
}

void LvParseTest::idObjectInJsScope(){
    parseTestTemplate("ParserTest24");
}

void LvParseTest::nestedObjectsInJsScope(){
    parseTestTemplate("ParserTest25");
}

void LvParseTest::sameIdInBothScopes(){
    parseTestTemplate("ParserTest26");
}

void LvParseTest::namespaceImportTest(){
    parseTestTemplate("ParserTest27");
}

void LvParseTest::multiNamespaceImportTest(){
    parseTestTemplate("ParserTest28");
}

void LvParseTest::customBaseComponentTest(){
    std::string name = "ParserTest29";
    std::string contents = m_fileIO->readFromFile(m_scriptPath + "/" + name + ".lv");
    std::string expect   = m_fileIO->readFromFile(m_scriptPath + "/" + name + ".lv.js");

    Compiler::Config compConfig = Compiler::Config();
    compConfig.setBaseComponent("CustomElement", "custom/CustomElement.js");

    Compiler::Ptr compiler = Compiler::create(compConfig);
    compiler->configureImplicitType("console");
    compiler->configureImplicitType("vlog");

    el::LanguageParser::Ptr parser = el::LanguageParser::createForElements();

    std::string conversion = compiler->compileToJs(m_scriptPath + "/" + name + ".lv.js", contents);

    el::LanguageParser::AST* conversionAST = parser->parse(conversion);
    el::LanguageParser::AST* expectedAST   = parser->parse(expect);

    el::LanguageParser::ComparisonResult compare = parser->compare(expect, expectedAST, conversion, conversionAST);
    parser->destroy(conversionAST);
    parser->destroy(expectedAST);

    QVERIFY(compare.isEqual());
}

void LvParseTest::defaultChildrenTest(){
    parseTestTemplate("ParserTest30");
}

void LvParseTest::bindShouldNotBeImportedTest(){
    parseTestTemplate("ParserTest31");
}

void LvParseTest::initializeElementTest(){
    parseTestTemplate("ParserTest32");
}

void LvParseTest::customConstructorArgumentsTest(){
    parseTestTemplate("ParserTest33");
}

void LvParseTest::unassignedPropertyTest(){
    parseTestTemplate("ParserTest34");
}

void LvParseTest::staticPropertyTest(){
    parseTestTemplate("ParserTest35");
}

void LvParseTest::functionAnnotationTest(){
    parseTestTemplate("ParserTest36");
}

void LvParseTest::jsImportsTest(){
    parseTestTemplate("ParserTest37");
}

void LvParseTest::trippleTagTest(){
    parseTestTemplate("ParserTest38");
}

void LvParseTest::componentImportTest(){
    parseTestTemplate("ParserTest39");
}

void LvParseTest::getterSetterTest(){
    parseTestTemplate("ParserTest40");
}

void LvParseTest::jsSectionsTest(){
    parseTestTemplate("ParserTest41");
}

void LvParseTest::eventsMethodsAndListenersTest(){
    parseTestTemplate("ParserTest42");
}

void LvParseTest::propertyAssignmentInBindingTest(){
    parseTestTemplate("ParserTest43");
}

void LvParseTest::instanceIdsTest(){
    parseTestTemplate("ParserTest44");
}

void LvParseTest::bindingChainTest(){
    parseTestTemplate("ParserTest45");
}

void LvParseTest::creationInMethodTest(){
    parseTestTemplate("ParserTest46");
}

void LvParseTest::propertyEqualsVsBinding(){
    parseTestTemplate("ParserTest47");
}

void LvParseTest::propertyNotation(){
    parseTestTemplate("ParserTest48");
}

void LvParseTest::parseTestTemplate(std::string name){
    try{
        std::string contents = m_fileIO->readFromFile(m_scriptPath + "/" + name + ".lv");
        std::string expect   = m_fileIO->readFromFile(m_scriptPath + "/" + name + ".lv.js");

        Compiler::Config compilerConfig(false);
        compilerConfig.allowUnresolvedTypes(true);
        Compiler::Ptr compiler = Compiler::create(compilerConfig);
        compiler->configureImplicitType("console");
        compiler->configureImplicitType("vlog");

        std::string conversion = compiler->compileToJs(m_scriptPath + "/" + name + ".lv", contents);


        Utf8::replaceAll(conversion, "\\r", "");

        el::LanguageParser::Ptr parser = el::LanguageParser::createForElements();
        el::LanguageParser::AST* conversionAST = parser->parse(conversion);
        el::LanguageParser::AST* expectedAST   = parser->parse(expect);

        el::LanguageParser::ComparisonResult compare = parser->compare(expect, expectedAST, conversion, conversionAST);
        parser->destroy(conversionAST);
        parser->destroy(expectedAST);

        if ( !compare.isEqual() ){
            vlog().e() << compare.errorString();
            vlog().e() << conversion;
        }
        QVERIFY(compare.isEqual());
    } catch ( lv::el::SyntaxException& e ){
        QFAIL(Utf8("SyntaxException: % at %:%").format(e.message(), e.parsedFile(), e.parsedLine()).data().c_str());
    } catch ( lv::Exception& e ){
        QFAIL(("Exception triggered: " + e.message()).c_str());
    }
}
