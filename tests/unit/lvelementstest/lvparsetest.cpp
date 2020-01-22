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
    std::string contents = m_fileSession->readFromFile(m_scriptPath + "/ParserTest1.lv");
    std::string expect   = m_fileSession->readFromFile(m_scriptPath + "/ParserTest1.lv.js");

    el::LanguageParser::Ptr parser = el::LanguageParser::createForElements();

    std::string conversion = parser->toJs(contents);

    el::LanguageParser::AST* conversionAST = parser->parse(conversion);
    el::LanguageParser::AST* expectedAST   = parser->parse(expect);

    el::LanguageParser::ComparisonResult compare = parser->compare(expect, expectedAST, conversion, conversionAST);

    parser->destroy(conversionAST);
    parser->destroy(expectedAST);

    QVERIFY(compare.isEqual());
}

void LvParseTest::functionParseTest(){
    std::string contents = m_fileSession->readFromFile(m_scriptPath + "/ParserTest2.lv");
    std::string expect   = m_fileSession->readFromFile(m_scriptPath + "/ParserTest2.lv.js");

    el::LanguageParser::Ptr parser = el::LanguageParser::createForElements();

    std::string conversion = parser->toJs(contents);

    el::LanguageParser::AST* conversionAST = parser->parse(conversion);
    el::LanguageParser::AST* expectedAST   = parser->parse(expect);

    el::LanguageParser::ComparisonResult compare = parser->compare(expect, expectedAST, conversion, conversionAST);

    parser->destroy(conversionAST);
    parser->destroy(expectedAST);

    QVERIFY(compare.isEqual());
}

void LvParseTest::nestingAndShortIdParseTest(){
    std::string contents = m_fileSession->readFromFile(m_scriptPath + "/ParserTest3.lv");
    std::string expect   = m_fileSession->readFromFile(m_scriptPath + "/ParserTest3.lv.js");

    el::LanguageParser::Ptr parser = el::LanguageParser::createForElements();

    std::string conversion = parser->toJs(contents);

    el::LanguageParser::AST* conversionAST = parser->parse(conversion);
    el::LanguageParser::AST* expectedAST   = parser->parse(expect);

    el::LanguageParser::ComparisonResult compare = parser->compare(expect, expectedAST, conversion, conversionAST);

    parser->destroy(conversionAST);
    parser->destroy(expectedAST);

    QVERIFY(compare.isEqual());
}

void LvParseTest::scopeParseTest(){
    std::string contents = m_fileSession->readFromFile(m_scriptPath + "/ParserTest4.lv");
    std::string expect   = m_fileSession->readFromFile(m_scriptPath + "/ParserTest4.lv.js");

    el::LanguageParser::Ptr parser = el::LanguageParser::createForElements();

    std::string conversion = parser->toJs(contents);

    el::LanguageParser::AST* conversionAST = parser->parse(conversion);
    el::LanguageParser::AST* expectedAST   = parser->parse(expect);

    el::LanguageParser::ComparisonResult compare = parser->compare(expect, expectedAST, conversion, conversionAST);

    parser->destroy(conversionAST);
    parser->destroy(expectedAST);

    QVERIFY(compare.isEqual());
}

void LvParseTest::propertyNewExpressionParseTest(){
    std::string contents = m_fileSession->readFromFile(m_scriptPath + "/ParserTest5.lv");
    std::string expect   = m_fileSession->readFromFile(m_scriptPath + "/ParserTest5.lv.js");

    el::LanguageParser::Ptr parser = el::LanguageParser::createForElements();

    std::string conversion = parser->toJs(contents, "ParserTest5");

    el::LanguageParser::AST* conversionAST = parser->parse(conversion);
    el::LanguageParser::AST* expectedAST   = parser->parse(expect);

    el::LanguageParser::ComparisonResult compare = parser->compare(expect, expectedAST, conversion, conversionAST);

    parser->destroy(conversionAST);
    parser->destroy(expectedAST);

    QVERIFY(compare.isEqual());
}

void LvParseTest::propertyDeclarationParseTest(){
    std::string contents = m_fileSession->readFromFile(m_scriptPath + "/ParserTest6.lv");
    std::string expect   = m_fileSession->readFromFile(m_scriptPath + "/ParserTest6.lv.js");

    el::LanguageParser::Ptr parser = el::LanguageParser::createForElements();

    std::string conversion = parser->toJs(contents, "ParserTest6");

    el::LanguageParser::AST* conversionAST = parser->parse(conversion);
    el::LanguageParser::AST* expectedAST   = parser->parse(expect);

    el::LanguageParser::ComparisonResult compare = parser->compare(expect, expectedAST, conversion, conversionAST);

    parser->destroy(conversionAST);
    parser->destroy(expectedAST);

    QVERIFY(compare.isEqual());
}

void LvParseTest::propertyExpressionsParseTest(){
    std::string contents = m_fileSession->readFromFile(m_scriptPath + "/ParserTest7.lv");
    std::string expect   = m_fileSession->readFromFile(m_scriptPath + "/ParserTest7.lv.js");

    el::LanguageParser::Ptr parser = el::LanguageParser::createForElements();

    std::string conversion = parser->toJs(contents, "ParserTest7");

    el::LanguageParser::AST* conversionAST = parser->parse(conversion);
    el::LanguageParser::AST* expectedAST   = parser->parse(expect);

    el::LanguageParser::ComparisonResult compare = parser->compare(expect, expectedAST, conversion, conversionAST);

    parser->destroy(conversionAST);
    parser->destroy(expectedAST);

    QVERIFY(compare.isEqual());
}

void LvParseTest::taggedStringParseTest(){
    std::string contents = m_fileSession->readFromFile(m_scriptPath + "/ParserTest8.lv");
    std::string expect   = m_fileSession->readFromFile(m_scriptPath + "/ParserTest8.lv.js");

    el::LanguageParser::Ptr parser = el::LanguageParser::createForElements();

    std::string conversion = parser->toJs(contents, "ParserTest8");

    el::LanguageParser::AST* conversionAST = parser->parse(conversion);
    el::LanguageParser::AST* expectedAST   = parser->parse(expect);

    el::LanguageParser::ComparisonResult compare = parser->compare(expect, expectedAST, conversion, conversionAST);

    parser->destroy(conversionAST);
    parser->destroy(expectedAST);

    QVERIFY(compare.isEqual());
}

void LvParseTest::instanceParseTest(){
    std::string contents = m_fileSession->readFromFile(m_scriptPath + "/ParserTest9.lv");
    std::string expect   = m_fileSession->readFromFile(m_scriptPath + "/ParserTest9.lv.js");

    el::LanguageParser::Ptr parser = el::LanguageParser::createForElements();

    std::string conversion = parser->toJs(contents, "ParserTest9");

    el::LanguageParser::AST* conversionAST = parser->parse(conversion);
    el::LanguageParser::AST* expectedAST   = parser->parse(expect);

    el::LanguageParser::ComparisonResult compare = parser->compare(expect, expectedAST, conversion, conversionAST);

    parser->destroy(conversionAST);
    parser->destroy(expectedAST);

    QVERIFY(compare.isEqual());
}

void LvParseTest::namespaceInheritance()
{
    std::string contents = m_fileSession->readFromFile(m_scriptPath + "/ParserTest10.lv");
    std::string expect   = m_fileSession->readFromFile(m_scriptPath + "/ParserTest10.lv.js");

    el::LanguageParser::Ptr parser = el::LanguageParser::createForElements();

    std::string conversion = parser->toJs(contents, "ParserTest10");

    el::LanguageParser::AST* conversionAST = parser->parse(conversion);
    el::LanguageParser::AST* expectedAST   = parser->parse(expect);

    el::LanguageParser::ComparisonResult compare = parser->compare(expect, expectedAST, conversion, conversionAST);

    parser->destroy(conversionAST);
    parser->destroy(expectedAST);

    QVERIFY(compare.isEqual());
}

void LvParseTest::testComplexPropertyAssignment()
{
    std::string contents = m_fileSession->readFromFile(m_scriptPath + "/ParserTest11.lv");
    std::string expect   = m_fileSession->readFromFile(m_scriptPath + "/ParserTest11.lv.js");

    el::LanguageParser::Ptr parser = el::LanguageParser::createForElements();

    std::string conversion = parser->toJs(contents, "ParserTest11");

    el::LanguageParser::AST* conversionAST = parser->parse(conversion);

    el::LanguageParser::AST* expectedAST   = parser->parse(expect);

    el::LanguageParser::ComparisonResult compare = parser->compare(expect, expectedAST, conversion, conversionAST);

    parser->destroy(conversionAST);
    parser->destroy(expectedAST);
}

void LvParseTest::testSimplePropertyAssignment()
{
    std::string contents = m_fileSession->readFromFile(m_scriptPath + "/ParserTest12.lv");
    std::string expect   = m_fileSession->readFromFile(m_scriptPath + "/ParserTest12.lv.js");

    el::LanguageParser::Ptr parser = el::LanguageParser::createForElements();

    std::string conversion = parser->toJs(contents, "ParserTest12");

    el::LanguageParser::AST* conversionAST = parser->parse(conversion);
    el::LanguageParser::AST* expectedAST   = parser->parse(expect);

    el::LanguageParser::ComparisonResult compare = parser->compare(expect, expectedAST, conversion, conversionAST);

    parser->destroy(conversionAST);
    parser->destroy(expectedAST);
}

void LvParseTest::testScenarioTest(){
    std::string contents = m_fileSession->readFromFile(m_scriptPath + "/ParserTest13.lv");
//    std::string expect   = m_fileSession->readFromFile(m_scriptPath + "/ParserTest10.lv.js");

    el::LanguageParser::Ptr parser = el::LanguageParser::createForElements();

    std::string conversion = parser->toJs(contents, "ParserTest11");

    el::LanguageParser::AST* conversionAST = parser->parse(conversion);

    vlog() << conversion;

//    el::LanguageParser::AST* expectedAST   = parser->parse(expect);

//    el::LanguageParser::ComparisonResult compare = parser->compare(expect, expectedAST, conversion, conversionAST);

//    parser->destroy(conversionAST);
//    parser->destroy(expectedAST);

//    QVERIFY(compare.isEqual());
}
