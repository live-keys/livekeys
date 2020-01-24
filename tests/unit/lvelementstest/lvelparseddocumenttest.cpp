#include "lvelparseddocumenttest.h"
#include <QDebug>
#include "live/elements/parseddocument.h"
#include "live/elements/languageinfo.h"
#include "live/utf8.h"
#include <vector>

Q_TEST_RUNNER_REGISTER(LvElParsedDocumentTest);

LvElParsedDocumentTest::LvElParsedDocumentTest(QObject* parent)
    : QObject(parent) {}

LvElParsedDocumentTest::~LvElParsedDocumentTest(){}

void LvElParsedDocumentTest::initTestCase()
{
    m_parser = lv::el::LanguageParser::createForElements();
}

void LvElParsedDocumentTest::extractImports1Test()
{
    std::string testString = "import a.b";
    auto ast = m_parser->parse(testString);

    std::vector<lv::el::ImportInfo> result = lv::el::ParsedDocument::extractImports(testString, ast);
    QVERIFY(result.size() == 1);

    auto res0 = result[0];
    QVERIFY(!res0.isRelative());
    QVERIFY(res0.segments().size() == 2);

    QVERIFY(res0.segments()[0] == "a");
    QVERIFY(res0.segments()[1] == "b");

    QVERIFY(res0.importAs() == "");
}

void LvElParsedDocumentTest::extractImports2Test()
{
    std::string testString = "import a\nimport .path.rel\n";
    auto ast = m_parser->parse(testString);
    std::vector<lv::el::ImportInfo> result = lv::el::ParsedDocument::extractImports(testString, ast);
    QVERIFY(result.size() == 2);

    auto res0 = result[0];
    QVERIFY(!res0.isRelative());
    QVERIFY(res0.segments().size() == 1);
    QVERIFY(res0.segments()[0] == "a");
    QVERIFY(res0.importAs() == "");

    auto res1 = result[1];
    QVERIFY(res1.isRelative());
    QVERIFY(res1.segments().size() == 2);
    QVERIFY(res1.segments()[0] == "path");
    QVERIFY(res1.segments()[1] == "rel");
    QVERIFY(res0.importAs() == "");
}

void LvElParsedDocumentTest::extractImports3Test()
{
    std::string testString = "import .path.rel.morerel as Test";
    auto ast = m_parser->parse(testString);
    std::vector<lv::el::ImportInfo> result = lv::el::ParsedDocument::extractImports(testString, ast);
    QVERIFY(result.size() == 1);

    auto res0 = result[0];
    QVERIFY(res0.isRelative());
    QVERIFY(res0.segments().size() == 3);
    QVERIFY(res0.importAs() == "Test");
}

void LvElParsedDocumentTest::extractImports4Test()
{
    std::string testString = "import .path.rel.morerel as Test\n"
                             "import a\n"
                             "import c.d\n"
                             "import proper.imports as imp\n"
                             "\n"
                             "component A { int a : b}\n"
                             "B {}";

    auto ast = m_parser->parse(testString);
    std::vector<lv::el::ImportInfo> result = lv::el::ParsedDocument::extractImports(testString, ast);
    QVERIFY(result.size() == 4);
}

void LvElParsedDocumentTest::extractInfo1Test()
{
    std::string testString = "import .path.rel.morerel as Test\n"
                             "\n"
                             "component A { int a : b; c: d }\n"
                             "B {}";

    auto ast = m_parser->parse(testString);
    lv::el::DocumentInfo::Ptr result = lv::el::ParsedDocument::extractInfo(testString, ast);

    QVERIFY(result->totalImports() == 1);
    QVERIFY(result->totalTypes() == 1);

    auto import = result->importAt(0);
    QVERIFY(import.isRelative());
    QVERIFY(import.segments().size() == 3);
    QVERIFY(import.segments()[0] == "path");
    QVERIFY(import.segments()[1] == "rel");
    QVERIFY(import.segments()[2] == "morerel");
    QVERIFY(import.importAs() == "Test");

    auto type = result->typeAt(0);
    QVERIFY(type->typeName() == "A");
    QVERIFY(type->inheritsName() == "Element");
    QVERIFY(!type->isInstance());
    QVERIFY(!type->isCreatable());
    QVERIFY(type->totalEvents() == 0);
    QVERIFY(type->totalFunctions() == 0);
    QVERIFY(type->totalProperties() == 1);
    QVERIFY(type->propertyAt(0).typeName() == "int");
    QVERIFY(type->propertyAt(0).name() == "a");

}

void LvElParsedDocumentTest::extractInfo2Test()
{
    std::string testString = "component A < Object { int a : b; c: d }\n"
                             "instance Insta B { int x: y\n"
                             "int z: t\n"
                             "event a(Object c)\n"
                             "}\n"
                             "B {}";

    auto ast = m_parser->parse(testString);
    lv::el::DocumentInfo::Ptr result = lv::el::ParsedDocument::extractInfo(testString, ast);

    QVERIFY(result->totalImports() == 0);
    QVERIFY(result->totalTypes() == 2);

    auto type = result->typeAt(0);
    QVERIFY(type->typeName() == "A");
    QVERIFY(type->inheritsName() == "Object");
    QVERIFY(!type->isInstance());
    QVERIFY(!type->isCreatable());
    QVERIFY(type->totalEvents() == 0);
    QVERIFY(type->totalFunctions() == 0);
    QVERIFY(type->totalProperties() == 1);
    QVERIFY(type->propertyAt(0).typeName() == "int");
    QVERIFY(type->propertyAt(0).name() == "a");


    auto type2 = result->typeAt(1);
    QVERIFY(type2->isInstance());
    QVERIFY(type2->typeName() == "B");
    QVERIFY(type2->inheritsName() == "Element");
    QVERIFY(!type2->isCreatable());

    QVERIFY(type2->totalEvents() == 1);
    QVERIFY(type2->eventAt(0).name() == "a");
    QVERIFY(type2->eventAt(0).parameterCount() == 1);
    QVERIFY(type2->eventAt(0).parameter(0).second == "Object");
    QVERIFY(type2->eventAt(0).parameter(0).first == "c");


    QVERIFY(type2->totalFunctions() == 0);

    QVERIFY(type2->totalProperties() == 2);
    QVERIFY(type2->propertyAt(0).typeName() == "int");
    QVERIFY(type2->propertyAt(0).name() == "x");
    QVERIFY(type2->propertyAt(1).typeName() == "int");
    QVERIFY(type2->propertyAt(1).name() == "z");
}

void LvElParsedDocumentTest::extractInfo3Test()
{
    std::string testString = "instance Insta B {\n"
                             "    int x: y\n"
                             "    z: t\n"
                             "    fn test(A a, B b, Object c) {}\n"
                             "    function ecmaTest(a, b){}\n"
                             "}\n"
                             "B {}";

    auto ast = m_parser->parse(testString);
    lv::el::DocumentInfo::Ptr result = lv::el::ParsedDocument::extractInfo(testString, ast);


    QVERIFY(result->totalImports() == 0);
    QVERIFY(result->totalTypes() == 1);

    auto type = result->typeAt(0);
    QVERIFY(type->typeName() == "B");
    QVERIFY(type->inheritsName() == "Element");
    QVERIFY(type->isInstance());
    QVERIFY(!type->isCreatable());
    QVERIFY(type->totalEvents() == 0);
    QVERIFY(type->totalFunctions() == 1);
    QVERIFY(type->totalProperties() == 1);
    QVERIFY(type->propertyAt(0).typeName() == "int");
    QVERIFY(type->propertyAt(0).name() == "x");
    QVERIFY(type->functionAt(0).name() == "test");
    QVERIFY(type->functionAt(0).returnType() == "");

    QVERIFY(type->functionAt(0).parameterCount() == 3);
    QVERIFY(type->functionAt(0).parameter(0).second == "A");
    QVERIFY(type->functionAt(0).parameter(0).first == "a");
    QVERIFY(type->functionAt(0).parameter(1).second == "B");
    QVERIFY(type->functionAt(0).parameter(1).first == "b");
    QVERIFY(type->functionAt(0).parameter(2).second == "Object");
    QVERIFY(type->functionAt(0).parameter(2).first == "c");


}

void LvElParsedDocumentTest::extractInfo4Test()
{
    std::string testString = "import .path.Cloud as C\n"
                             "component B < C.Element {\n"
                             "    int x: y\n"
                             "    z: t\n"
                             "    fn test(A a, B b, Object c) {}\n"
                             "    constructor(m,n,p){/*body*/} \n"
                             "    function ecmaTest(a, b){}\n"
                             "}\n"
                             "B {}";

    auto ast = m_parser->parse(testString);
    lv::el::DocumentInfo::Ptr result = lv::el::ParsedDocument::extractInfo(testString, ast);

    QVERIFY(result->totalImports() == 1);
    QVERIFY(result->totalTypes() == 1);

    auto type = result->typeAt(0);
    QVERIFY(type->typeName() == "B");
    QVERIFY(type->inheritsName() == "C.Element");
    QVERIFY(!type->isInstance());
    QVERIFY(!type->isCreatable());
    QVERIFY(type->totalEvents() == 0);
    QVERIFY(type->totalFunctions() == 1);
    QVERIFY(type->totalProperties() == 1);
    QVERIFY(type->propertyAt(0).typeName() == "int");
    QVERIFY(type->propertyAt(0).name() == "x");
    QVERIFY(type->functionAt(0).name() == "test");
    QVERIFY(type->functionAt(0).returnType() == "");
}

void LvElParsedDocumentTest::cursorContext1Test()
{
    std::string testString = "import path.to.import";

    auto ast = m_parser->parse(testString);
    lv::el::CursorContext result = lv::el::ParsedDocument::findCursorContext(ast, 17);

    QVERIFY(result.context() == (lv::el::CursorContext::InImport | lv::el::CursorContext::InElements));
    auto path = result.expressionPath();
    QVERIFY(path.size() == 3);

    QVERIFY(path[0].from() == 7);
    QVERIFY(path[0].length() == 4);

    QVERIFY(path[1].from() == 12);
    QVERIFY(path[1].length() == 2);

    QVERIFY(path[2].from() == 15);
    QVERIFY(path[2].length() == 2);

    QVERIFY(!result.objectType().isValid());
    QVERIFY(!result.objectImportNamespace().isValid());
    QVERIFY(result.propertyPath().size() == 0);
    QVERIFY(!result.propertyDeclaredType().isValid());
}

void LvElParsedDocumentTest::cursorContext2Test()
{
    std::string testString = "import path.to.import";

    auto ast = m_parser->parse(testString);
    lv::el::CursorContext result = lv::el::ParsedDocument::findCursorContext(ast, 15);

    QVERIFY(result.context() == (lv::el::CursorContext::InImport | lv::el::CursorContext::InElements));
    auto path = result.expressionPath();
    QVERIFY(path.size() == 2);

    QVERIFY(path[0].from() == 7);
    QVERIFY(path[0].length() == 4);

    QVERIFY(path[1].from() == 12);
    QVERIFY(path[1].length() == 2);

    QVERIFY(!result.objectType().isValid());
    QVERIFY(!result.objectImportNamespace().isValid());
    QVERIFY(result.propertyPath().size() == 0);
    QVERIFY(!result.propertyDeclaredType().isValid());
}

void LvElParsedDocumentTest::cursorContext3Test()
{
    std::string testString = "import path.to.import";

    auto ast = m_parser->parse(testString);
    lv::el::CursorContext result = lv::el::ParsedDocument::findCursorContext(ast, 7);

    QVERIFY(result.context() == (lv::el::CursorContext::InImport | lv::el::CursorContext::InElements));
    auto path = result.expressionPath();
    QVERIFY(path.size() == 0);
    QVERIFY(!result.objectType().isValid());
    QVERIFY(!result.objectImportNamespace().isValid());
    QVERIFY(result.propertyPath().size() == 0);
    QVERIFY(!result.propertyDeclaredType().isValid());
}

void LvElParsedDocumentTest::cursorContext4Test()
{
    std::string testString = "import path.to.import";

    auto ast = m_parser->parse(testString);
    lv::el::CursorContext result = lv::el::ParsedDocument::findCursorContext(ast, 5);

    QVERIFY(result.context() == 0);
    auto path = result.expressionPath();

    QVERIFY(path.size() == 1); // import keyword!
    QVERIFY(!result.objectType().isValid());
    QVERIFY(!result.objectImportNamespace().isValid());
    QVERIFY(result.propertyPath().size() == 0);
    QVERIFY(!result.propertyDeclaredType().isValid());
}

void LvElParsedDocumentTest::cursorContext5Test()
{
    std::string testString = "component A {\n"
                             "   function a(){\n"
                             "       var b =\"stringtest\""
                             "   }"
                             "";

    auto ast = m_parser->parse(testString);
    lv::el::CursorContext result = lv::el::ParsedDocument::findCursorContext(ast, 52);

    QVERIFY(result.context() == (lv::el::CursorContext::InStringLiteral | lv::el::CursorContext::InElements));
    auto path = result.expressionPath();
    QVERIFY(path.size() == 0);
    QVERIFY(!result.objectType().isValid());
    QVERIFY(!result.objectImportNamespace().isValid());
    QVERIFY(result.propertyPath().size() == 0);
    QVERIFY(!result.propertyDeclaredType().isValid());
}

void LvElParsedDocumentTest::cursorContext6Test()
{
    std::string testString = "component T < Element{ prop : 200 }";

    auto ast = m_parser->parse(testString);
    lv::el::CursorContext result = lv::el::ParsedDocument::findCursorContext(ast, 25);

    QVERIFY(result.context() == (lv::el::CursorContext::InLeftOfDeclaration | lv::el::CursorContext::InElements));
    auto path = result.expressionPath();

    QVERIFY(path.size() == 1);

    QVERIFY(path[0].from() == 23);
    QVERIFY(path[0].length() == 2);

    QVERIFY(result.objectType().from() == 14);
    QVERIFY(result.objectType().length() == 7);
    QVERIFY(!result.objectImportNamespace().isValid());
    QVERIFY(result.propertyPath().size() == 0);
    QVERIFY(!result.propertyDeclaredType().isValid());
}

void LvElParsedDocumentTest::cursorContext7Test()
{
    std::string testString = "component T < Element{ int prop : 200 }";

    auto ast = m_parser->parse(testString);
    lv::el::CursorContext result = lv::el::ParsedDocument::findCursorContext(ast, 29);

    QVERIFY(result.context() == (lv::el::CursorContext::InLeftOfDeclaration | lv::el::CursorContext::InElements));
    auto path = result.expressionPath();

    QVERIFY(path.size() == 1);

    QVERIFY(path[0].from() == 27);
    QVERIFY(path[0].length() == 2);

    QVERIFY(result.objectType().from() == 14);
    QVERIFY(result.objectType().length() == 7);

    QVERIFY(result.propertyDeclaredType().from() == 23);
    QVERIFY(result.propertyDeclaredType().length() == 3);

    QVERIFY(!result.objectImportNamespace().isValid());
    QVERIFY(result.propertyPath().size() == 0);
}

void LvElParsedDocumentTest::cursorContext8Test()
{
    std::string testString = "component T < Element{ on complete: () => {} }";

    auto ast = m_parser->parse(testString);
    lv::el::CursorContext result = lv::el::ParsedDocument::findCursorContext(ast, 29);

    QVERIFY(result.context() == (lv::el::CursorContext::InLeftOfDeclaration | lv::el::CursorContext::InElements));
    auto path = result.expressionPath();
    QVERIFY(path.size() == 1);

    QVERIFY(path[0].from() == 26);
    QVERIFY(path[0].length() == 3);

    QVERIFY(result.objectType().from() == 14);
    QVERIFY(result.objectType().length() == 7);

    QVERIFY(result.propertyDeclaredType().from() == 23);
    QVERIFY(result.propertyDeclaredType().length() == 2);

    QVERIFY(!result.objectImportNamespace().isValid());
    QVERIFY(result.propertyPath().size() == 0);
}

void LvElParsedDocumentTest::cursorContext9Test()
{
    std::string testString = "T{ prop : 200 }";

    auto ast = m_parser->parse(testString);
    lv::el::CursorContext result = lv::el::ParsedDocument::findCursorContext(ast, 5);

    QVERIFY(result.context() == (lv::el::CursorContext::InLeftOfDeclaration | lv::el::CursorContext::InElements));
    auto path = result.expressionPath();
    QVERIFY(path.size() == 1);

    QVERIFY(path[0].from() == 3);
    QVERIFY(path[0].length() == 2);

    QVERIFY(result.objectType().from() == 0);
    QVERIFY(result.objectType().length() == 1);

    QVERIFY(!result.propertyDeclaredType().isValid());
    QVERIFY(!result.objectImportNamespace().isValid());
    QVERIFY(result.propertyPath().size() == 0);
}

void LvElParsedDocumentTest::cursorContext10Test()
{
    std::string testString = "T{ prop : this.font.size }";

    auto ast = m_parser->parse(testString);
    lv::el::CursorContext result = lv::el::ParsedDocument::findCursorContext(ast, 22);

    QVERIFY(result.context() == (lv::el::CursorContext::InRightOfDeclaration | lv::el::CursorContext::InElements));
    auto path = result.expressionPath();
    QVERIFY(path.size() == 3);

    QVERIFY(path[0].from() == 10);
    QVERIFY(path[0].length() == 4);
    QVERIFY(path[1].from() == 15);
    QVERIFY(path[1].length() == 4);
    QVERIFY(path[2].from() == 20);
    QVERIFY(path[2].length() == 2);


    QVERIFY(result.objectType().from() == 0);
    QVERIFY(result.objectType().length() == 1);

    auto propPath = result.propertyPath();
    QVERIFY(propPath.size() == 1);
    QVERIFY(propPath[0].from() == 3);
    QVERIFY(propPath[0].length() == 4);

    QVERIFY(!result.objectImportNamespace().isValid());
    QVERIFY(!result.propertyDeclaredType().isValid());
}

void LvElParsedDocumentTest::cursorContext11Test()
{
    std::string testString = "T{ prop : this.font.size }";

    auto ast = m_parser->parse(testString);
    lv::el::CursorContext result = lv::el::ParsedDocument::findCursorContext(ast, 10);

    QVERIFY(result.context() == (lv::el::CursorContext::InRightOfDeclaration | lv::el::CursorContext::InElements));
    auto path = result.expressionPath();
    QVERIFY(path.size() == 0);

    QVERIFY(result.objectType().from() == 0);
    QVERIFY(result.objectType().length() == 1);

    auto propPath = result.propertyPath();
    QVERIFY(propPath.size() == 1);
    QVERIFY(propPath[0].from() == 3);
    QVERIFY(propPath[0].length() == 4);

    QVERIFY(!result.objectImportNamespace().isValid());
    QVERIFY(!result.propertyDeclaredType().isValid());
}

void LvElParsedDocumentTest::cursorContext12Test()
{
    std::string testString = "import view as View\n"
                             "View.T{ prop : 200}";

    auto ast = m_parser->parse(testString);
    lv::el::CursorContext result = lv::el::ParsedDocument::findCursorContext(ast, 30);


    QVERIFY(result.context() == (lv::el::CursorContext::InLeftOfDeclaration | lv::el::CursorContext::InElements));
    auto path = result.expressionPath();
    QVERIFY(path.size() == 1);


    QVERIFY(path[0].from() == 28);
    QVERIFY(path[0].length() == 2);

    QVERIFY(result.objectType().from() == 25);
    QVERIFY(result.objectType().length() == 1);

    QVERIFY(result.objectImportNamespace().from() == 20);
    QVERIFY(result.objectImportNamespace().length() == 4);

    QVERIFY(result.propertyPath().size() == 0);

    QVERIFY(!result.propertyDeclaredType().isValid());
}

void LvElParsedDocumentTest::cursorContext13Test()
{
    std::string testString = "T{\n"
                             "    fn declaredFunction(){\n"
                             "        return 0\n"
                             "    }\n"
                             "}";

    auto ast = m_parser->parse(testString);
    lv::el::CursorContext result = lv::el::ParsedDocument::findCursorContext(ast, 43);

    QVERIFY(result.context() == (lv::el::CursorContext::InElements)); // \ inrightofdeclaration
    auto path = result.expressionPath();
    QVERIFY(path.size() == 1);


    QVERIFY(path[0].from() == 38);
    QVERIFY(path[0].length() == 5);

    QVERIFY(result.objectType().from() == 0);
    QVERIFY(result.objectType().length() == 1);

    QVERIFY(!result.objectImportNamespace().isValid());
    QVERIFY(result.propertyPath().size() == 0);
    QVERIFY(!result.propertyDeclaredType().isValid());
}
