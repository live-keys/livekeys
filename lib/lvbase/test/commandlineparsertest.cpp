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

#include "live/commandlineparser.h"
#include "catch_amalgamated.hpp"

using namespace lv;

TEST_CASE( "CommandLineParser Test", "[CommandLineParser]" ) {
    SECTION("Test Version Flag"){
        CommandLineParser parser("Header");
        const char *argv[] = {"<bin>", "--help"};
        parser.parse(2, argv);
        REQUIRE(parser.isSet(parser.helpOption()) == true);
        REQUIRE(parser.isSet(parser.versionOption()) == false);
    }
    SECTION("Test Help Flag"){
        CommandLineParser parser("Header");
        const char *argv[] = {"<bin>", "-v"};
        parser.parse(2, argv);
        REQUIRE(parser.isSet(parser.helpOption()) == false);
        REQUIRE(parser.isSet(parser.versionOption()) == true);
    }
    SECTION("Test No Flag"){
        CommandLineParser parser("Header");
        CommandLineParser::Option* debugOption = parser.addFlag({"-d"}, "debug");
        CommandLineParser::Option* infoOption  = parser.addFlag({"-i"}, "info");

        const char *argv[] = {"<bin>"};
        parser.parse(1, argv);
        REQUIRE(parser.isSet(debugOption) == false);
        REQUIRE(parser.isSet(infoOption) == false);
    }
    SECTION("Test MultiName Flag"){
        CommandLineParser parser("Header");
        CommandLineParser::Option* debugOption = parser.addFlag({"-d"}, "debug");
        CommandLineParser::Option* infoOption  = parser.addFlag({"-i", "--info"}, "info");

        const char *argv[] = {"<bin>", "--info", "-d"};
        parser.parse(3, argv);
        REQUIRE(parser.isSet(infoOption) == true);
        REQUIRE(parser.isSet(debugOption) == true);
    }
    SECTION("No Option Test"){
        CommandLineParser parser("Header");
        CommandLineParser::Option* testOption = parser.addOption({"--opt"}, "Test Option", "string");
        const char *argv[] = {"<bin>"};
        parser.parse(1, argv);
        REQUIRE(parser.isSet(testOption) == false);
        REQUIRE_THROWS_AS(parser.assertIsSet(testOption), CommandLineParserException);
    }
    SECTION("Option Test"){
        CommandLineParser parser("Header");
        CommandLineParser::Option* testOption = parser.addOption({"--opt"}, "Test Option", "string");
        const char *argv[] = {"<bin>", "--opt", "test"};
        parser.parse(3, argv);
        REQUIRE(parser.isSet(testOption) == true);
        REQUIRE(parser.value(testOption) == "test");
    }
    SECTION("Option and Flag Test"){
        CommandLineParser parser("Header");
        CommandLineParser::Option* testOption = parser.addOption({"--opt"}, "Test Option", "string");
        CommandLineParser::Option* infoOption = parser.addFlag({"--info"}, "Info Option");
        const char *argv[] = {"<bin>", "--opt", "test", "--info"};
        parser.parse(4, argv);
        REQUIRE(parser.isSet(infoOption) == true);
        REQUIRE(parser.isSet(testOption) == true);
        REQUIRE(parser.value(testOption) == "test");
    }
    SECTION("Invalid Option Test"){
        CommandLineParser parser("Header");
        parser.addOption({"--opt"}, "Test Option", "string");
        parser.addFlag({"--info"}, "Info Option");
        const char *argv[] = {"<bin>", "--opt"};
        REQUIRE_THROWS_AS(parser.parse(2, argv), CommandLineParserException);
    }
}
