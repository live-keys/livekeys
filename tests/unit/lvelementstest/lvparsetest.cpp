#include "lvparsetest.h"
#include "live/lockedfileiosession.h"
#include "live/applicationcontext.h"

#include "live/elements/element.h"
#include "live/elements/metaobject.h"
#include "live/elements/component.h"
#include "live/elements/modulelibrary.h"
#include "live/elements/languageparser.h"

Q_TEST_RUNNER_REGISTER(LvParseTest);

using namespace lv;
using namespace lv::el;

LvParseTest::LvParseTest(QObject *parent)
    : QObject(parent)
{
}

void LvParseTest::initTestCase(){
    m_fileSession = LockedFileIOSession::createInstance();
    m_scriptPath  = lv::ApplicationContext::instance().releasePath() + "/data";
}


void LvParseTest::constructorParseTest(){
    parseTestTemplate("ParserTest1");
}

void LvParseTest::functionParseTest(){
    parseTestTemplate("ParserTest2");
}

void LvParseTest::nestingAndShortIdParseTest(){
    parseTestTemplate("ParserTest3");
}

void LvParseTest::scopeParseTest(){
    parseTestTemplate("ParserTest4");
}

void LvParseTest::propertyNewExpressionParseTest(){
    parseTestTemplate("ParserTest5");
}

void LvParseTest::propertyDeclarationParseTest(){
    parseTestTemplate("ParserTest6");
}

void LvParseTest::propertyExpressionsParseTest(){
    parseTestTemplate("ParserTest7");
}

void LvParseTest::taggedStringParseTest(){
    parseTestTemplate("ParserTest8");
}

void LvParseTest::instanceParseTest(){
    parseTestTemplate("ParserTest9");
}

void LvParseTest::namespaceInheritance()
{
    parseTestTemplate("ParserTest10");
}

void LvParseTest::testComplexPropertyAssignment()
{
    parseTestTemplate("ParserTest11");
}

void LvParseTest::testSimplePropertyAssignment()
{
    parseTestTemplate("ParserTest12");
}

void LvParseTest::testScenarioTest(){
    parseTestTemplate("ParserTest13");
}

void LvParseTest::nestedLanguageScannerTest(){
    parseTestTemplate("ParserTest14");
}

void LvParseTest::nestedFunctionAssignment()
{
    parseTestTemplate("ParserTest15");
}

void LvParseTest::arrowFunctionElement()
{
    parseTestTemplate("ParserTest16");
}

void LvParseTest::todoListItemSmall()
{
    parseTestTemplate("ParserTest17");
}

void LvParseTest::todoList()
{
    parseTestTemplate("ParserTest18");
}

void LvParseTest::todoListItem()
{
    parseTestTemplate("ParserTest19");
}


void LvParseTest::todoForm()
{
    parseTestTemplate("ParserTest20");
}


void LvParseTest::todoApp()
{
    parseTestTemplate("ParserTest21");
}

void LvParseTest::complexTernaryOperator()
{
    parseTestTemplate("ParserTest22");
}

void LvParseTest::doublyNestedElement()
{
    parseTestTemplate("ParserTest23");
}

void LvParseTest::parseTestTemplate(std::string name)
{
    std::string contents = m_fileSession->readFromFile(m_scriptPath + "/" + name + ".lv");
    std::string expect   = m_fileSession->readFromFile(m_scriptPath + "/" + name + ".lv.js");

    el::LanguageParser::Ptr parser = el::LanguageParser::createForElements();

    std::string conversion = parser->toJs(contents, name);

    el::LanguageParser::AST* conversionAST = parser->parse(conversion);
    el::LanguageParser::AST* expectedAST   = parser->parse(expect);

    el::LanguageParser::ComparisonResult compare = parser->compare(expect, expectedAST, conversion, conversionAST);

    parser->destroy(conversionAST);
    parser->destroy(expectedAST);

    QVERIFY(compare.isEqual());
}
