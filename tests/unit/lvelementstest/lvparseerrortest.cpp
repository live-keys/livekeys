#include "lvparseerrortest.h"
#include "live/applicationcontext.h"

#include "live/elements/compiler/languageparser.h"
#include "live/elements/element.h"
#include "live/elements/metaobject.h"
#include "live/elements/component.h"
#include "live/elements/modulelibrary.h"

Q_TEST_RUNNER_REGISTER(LvParseErrorTest);

using namespace lv;
using namespace lv::el;

LvParseErrorTest::LvParseErrorTest(QObject *parent)
    : QObject(parent)
{
}

void LvParseErrorTest::initTestCase(){
    m_fileIO = new FileIO;
    m_scriptPath = Path::parent(lv::ApplicationContext::instance().applicationFilePath()) + "/data";
}

void LvParseErrorTest::programBodyExtraElements(){
    std::string name = "ParserErrorTest01";
    std::string contents = m_fileIO->readFromFile(m_scriptPath + "/" + name + ".lv");

    Compiler::Ptr compiler = Compiler::create();
    compiler->configureImplicitType("console");
    compiler->configureImplicitType("vlog");

    bool hadException = false;

    try {
        compiler->compileToJs(m_scriptPath + "/" + name + ".lv", contents);
    } catch (lv::el::SyntaxException& e) {
        QVERIFY(e.parsedLine() == 4);
        QVERIFY(e.parsedColumn() == 1);
        QVERIFY(e.code() == lv::Exception::toCode("~Parse"));
        hadException = true;
    }
    QVERIFY(hadException);
}

void LvParseErrorTest::identifierNotFound(){
    std::string name = "ParserErrorTest02";
    std::string contents = m_fileIO->readFromFile(m_scriptPath + "/" + name + ".lv");

    Compiler::Config compilerConfig;
    compilerConfig.allowUnresolvedTypes(false);
    Compiler::Ptr compiler = Compiler::create(compilerConfig);
    compiler->configureImplicitType("console");
    compiler->configureImplicitType("vlog");

    bool hadException = false;

    try {
        compiler->compileToJs(m_scriptPath + "/" + name + ".lv", contents);
    } catch (lv::Exception& e) {
        QVERIFY(e.code() == lv::Exception::toCode("~Identifier"));
        hadException = true;
    }
    QVERIFY(hadException);

}
