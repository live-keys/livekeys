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

FileIO& fileIO(){
    static FileIO fileio;
    return fileio;
}

const std::string& scriptPath(){
    static std::string scriptPath = Path::join(Path::parent(lv::ApplicationContext::instance().applicationFilePath()), "data");;
    return scriptPath;
}



TEST_CASE( "Parse Error Test", "[ParseError]" ) {

    SECTION("Program Body Extra Elements"){
        std::string name = "ParserErrorTest01";
        std::string filePath = scriptPath() + "/" + name + ".lv";
        std::string contents = fileIO().readFromFile(filePath);

        Compiler::Ptr compiler = Compiler::create();
        compiler->configureImplicitType("console");
        compiler->configureImplicitType("vlog");

        bool hadException = false;

        try {
            compiler->compileToJs(filePath, contents);
        } catch (lv::el::SyntaxException& e) {
            REQUIRE(e.code() == lv::Exception::toCode("~Language"));
            hadException = true;
        }
        REQUIRE(hadException);
    }
    SECTION("Identifier Not Found"){
        std::string name = "ParserErrorTest02";
        std::string filePath = scriptPath() + "/" + name + ".lv";
        std::string contents = fileIO().readFromFile(scriptPath() + "/" + name + ".lv");

        Compiler::Config compilerConfig;
        compilerConfig.allowUnresolvedTypes(false);
        Compiler::Ptr compiler = Compiler::create(compilerConfig);
        compiler->configureImplicitType("console");
        compiler->configureImplicitType("vlog");

        bool hadException = false;

        try {
            compiler->compileToJs(filePath, contents);
        } catch (lv::Exception& e) {
            REQUIRE(e.code() == lv::Exception::toCode("~Identifier"));
            hadException = true;
        }
        REQUIRE(hadException);
    }
}

