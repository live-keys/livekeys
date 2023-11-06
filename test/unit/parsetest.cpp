/****************************************************************************
**
** Copyright (C) 2022 Dinu SV.
**
** This file is part of Livekeys Application.
**
** GNU Lesser General Public License Usage
** This file may be used under the terms of the GNU Lesser
** General Public License version 3 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPLv3 included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 3 requirements
** will be met: https://www.gnu.org/licenses/lgpl.html.
**
****************************************************************************/

#include "catch_library.h"
#include "live/fileio.h"
#include "live/visuallog.h"
#include "live/datetime.h"
#include "live/applicationcontext.h"

#include "live/elements/compiler/languageparser.h"
#include "live/elements/compiler/compiler.h"

using namespace lv;
using namespace lv::el;

void testFileParse(const std::string& name){
    static FileIO fileIO;
    static std::string scriptPath = Path::join(Path::parent(lv::ApplicationContext::instance().applicationFilePath()), "data");

    try{
        std::string contents = fileIO.readFromFile(Path::join(scriptPath, name + ".lv"));

        std::vector<std::string> expectations = {
            ".js",
            ".ts",
        };

        for(const auto& expectation : expectations) {
            const auto expectationPath = Path::join(scriptPath, name + ".lv" + expectation);
            if (!Path::exists(expectationPath)) continue;
            const auto expectedContent = fileIO.readFromFile(expectationPath);
            Compiler::Config compilerConfig(false);
            compilerConfig.allowUnresolvedTypes(true);
            compilerConfig.outputTypes(expectation == ".ts");
            Compiler::Ptr compiler = Compiler::create(compilerConfig);
            compiler->configureImplicitType("console");
            compiler->configureImplicitType("vlog");

            std::string conversion = compiler->compileToJs(Path::join(scriptPath, name + ".lv"), contents);

            el::LanguageParser::Ptr parser = el::LanguageParser::createForElements();
            el::LanguageParser::AST* conversionAST = parser->parse(conversion);
            el::LanguageParser::AST* expectedAST   = parser->parse(expectedContent);

            el::LanguageParser::ComparisonResult compare = parser->compare(expectedContent, expectedAST, conversion, conversionAST);
            parser->destroy(conversionAST);
            parser->destroy(expectedAST);

            if ( !compare.isEqual() ){
                vlog().e() << "File: " << name << ".lv" << expectation;
                vlog().e() << compare.errorString();
                vlog().e() << conversion;
            }
            REQUIRE(compare.isEqual());
        }


    } catch ( lv::el::SyntaxException& e ){
        FAIL(Utf8("SyntaxException: % at %:%").format(e.message(), e.parsedFile(), e.parsedLine()).data().c_str());
    } catch ( lv::Exception& e ){
        FAIL(("Exception triggered: " + e.message()).c_str());
    }
}



TEST_CASE( "Parse Test", "[Parse]" ) {
    SECTION("Constructor"){ testFileParse("ParserTest01"); }
    SECTION("Function"){ testFileParse("ParserTest02"); }
    SECTION("Nesting and Short Id"){ testFileParse("ParserTest03"); }
    SECTION("Scope"){ testFileParse("ParserTest04"); }
    SECTION("Property New Expression"){ testFileParse("ParserTest05"); }
    SECTION("Property Declaration"){ testFileParse("ParserTest06"); }
    SECTION("Property Expressions"){ testFileParse("ParserTest07"); }
    SECTION("Tagged String"){ testFileParse("ParserTest08"); }
    SECTION("Instance"){ testFileParse("ParserTest09"); }

    SECTION("Namespace Inheritance"){ testFileParse("ParserTest10"); }
    SECTION("Complex Property Assignment"){ testFileParse("ParserTest11"); }
    SECTION("Simple Property Assignments"){ testFileParse("ParserTest12"); }
    SECTION("Test Scenario Parse"){ testFileParse("ParserTest13"); }
    SECTION("Nested Language Scanner"){ testFileParse("ParserTest14"); }
    SECTION("Nested Function Assignment"){ testFileParse("ParserTest15"); }
    SECTION("Arrow Function"){ testFileParse("ParserTest16"); }
    SECTION("TodoList Item Small Sample"){ testFileParse("ParserTest17"); }
    SECTION("TodoList Sample"){ testFileParse("ParserTest18"); }
    SECTION("TodoList Item Sample"){ testFileParse("ParserTest19"); }

    SECTION("Todo Form Sample"){ testFileParse("ParserTest20"); }
    SECTION("TodoApp Sample"){ testFileParse("ParserTest21"); }
    SECTION("Complex Ternary Operator"){ testFileParse("ParserTest22"); }
    SECTION("Double Nested Element"){ testFileParse("ParserTest23"); }
    SECTION("Id Object in Js Scope"){ testFileParse("ParserTest24"); }
    SECTION("Nested Objects in Js Scope"){ testFileParse("ParserTest25"); }
    SECTION("Same Id in Both Scopes"){ testFileParse("ParserTest26"); }
    SECTION("Namespace Import"){ testFileParse("ParserTest27"); }
    SECTION("Multi Namespace Import"){ testFileParse("ParserTest28"); }
    SECTION("Custom Base Component"){
        FileIO fileIO;
        static std::string scriptPath = Path::join(Path::parent(lv::ApplicationContext::instance().applicationFilePath()), "data");

        try{
            std::string name = "ParserTest29";
            std::string contents = fileIO.readFromFile(Path::join(scriptPath, name + ".lv"));
            std::string expect   = fileIO.readFromFile(Path::join(scriptPath, name + ".lv.js"));

            Compiler::Config compConfig = Compiler::Config();
            compConfig.setBaseComponent("CustomElement", "custom/CustomElement.js");

            Compiler::Ptr compiler = Compiler::create(compConfig);
            compiler->configureImplicitType("console");
            compiler->configureImplicitType("vlog");

            el::LanguageParser::Ptr parser = el::LanguageParser::createForElements();

            std::string conversion = compiler->compileToJs(Path::join(scriptPath, name + ".lv.js"), contents);

            el::LanguageParser::AST* conversionAST = parser->parse(conversion);
            el::LanguageParser::AST* expectedAST   = parser->parse(expect);

            el::LanguageParser::ComparisonResult compare = parser->compare(expect, expectedAST, conversion, conversionAST);
            parser->destroy(conversionAST);
            parser->destroy(expectedAST);

            REQUIRE(compare.isEqual());
        } catch ( lv::el::SyntaxException& e ){
            FAIL(Utf8("SyntaxException: % at %:%").format(e.message(), e.parsedFile(), e.parsedLine()).data().c_str());
        } catch ( lv::Exception& e ){
            FAIL(("Exception triggered: " + e.message()).c_str());
        }
    }

    SECTION("Default Children"){ testFileParse("ParserTest30"); }
    SECTION("Bind Should Not Be Imported"){ testFileParse("ParserTest31"); }
    SECTION("Initialize Element"){ testFileParse("ParserTest32"); }
    SECTION("Custom Constructor Arguments"){ testFileParse("ParserTest33"); }
    SECTION("Unassigned Property"){ testFileParse("ParserTest34"); }
    SECTION("Static Property"){ testFileParse("ParserTest35"); }
    SECTION("Function Annotation"){ testFileParse("ParserTest36"); }
    SECTION("Js Imports"){ testFileParse("ParserTest37"); }
    SECTION("Tripple Tag"){ testFileParse("ParserTest38"); }
    SECTION("Component Import"){ testFileParse("ParserTest39"); }

    SECTION("Getter Setter"){ testFileParse("ParserTest40"); }
    SECTION("Js Sections"){ testFileParse("ParserTest41"); }
    SECTION("Events Methods and Listeners"){ testFileParse("ParserTest42"); }
    SECTION("Property Assignment in Binding"){ testFileParse("ParserTest43"); }
    SECTION("Instance Ids"){ testFileParse("ParserTest44"); }
    SECTION("Binding Chain"){ testFileParse("ParserTest45"); }
    SECTION("Creation In Method"){ testFileParse("ParserTest46"); }
    SECTION("Property Equals vs Property Binding"){ testFileParse("ParserTest47"); }
    SECTION("Property Notation"){ testFileParse("ParserTest48"); }
    SECTION("Anonymous Components"){ testFileParse("ParserTest49"); }
    SECTION("Additional Declarations"){ testFileParse("ParserTest50"); }
    SECTION("Function & Variables Type Test"){ testFileParse("ParserTypeTest01"); }
}

