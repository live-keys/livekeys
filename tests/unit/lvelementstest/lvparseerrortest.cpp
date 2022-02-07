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
    m_fileSession = LockedFileIOSession::createInstance();
    m_scriptPath  = lv::ApplicationContext::instance().releasePath() + "/data";
}

void LvParseErrorTest::programBodyExtraElements(){
    std::string name = "ParserErrorTest01";
    std::string contents = m_fileSession->readFromFile(m_scriptPath + "/" + name + ".lv");

    Compiler::Ptr compiler = Compiler::create();
    compiler->configureImplicitType("console");
    compiler->configureImplicitType("vlog");

    bool hadException = false;

    try {
        compiler->compileToJs(name, contents);
    } catch (lv::el::SyntaxException& e) {
        QVERIFY(e.parsedLine() == 3);
        QVERIFY(e.parsedColumn() == 0);
        QVERIFY(e.code() == lv::Exception::toCode("~Parse"));
        hadException = true;
    }
    QVERIFY(hadException);
}
