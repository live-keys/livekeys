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

#include "commandlineparsertest.h"
#include "live/commandlineparser.h"
#include <QtTest/QtTest>

Q_TEST_RUNNER_REGISTER(CommandLineParserTest);

using namespace lv;

CommandLineParserTest::CommandLineParserTest(QObject *parent)
    : QObject(parent)
{
}

void CommandLineParserTest::initTestCase(){
}

void CommandLineParserTest::versionFlagTest(){
    CommandLineParser parser("Header");
    const char *argv[] = {"<bin>", "--help"};
    parser.parse(2, argv);
    QCOMPARE(parser.isSet(parser.helpOption()), true);
    QCOMPARE(parser.isSet(parser.versionOption()), false);
}

void CommandLineParserTest::helpFlagTest(){
    CommandLineParser parser("Header");
    const char *argv[] = {"<bin>", "-v"};
    parser.parse(2, argv);
    QCOMPARE(parser.isSet(parser.helpOption()), false);
    QCOMPARE(parser.isSet(parser.versionOption()), true);
}

void CommandLineParserTest::noFlagTest(){
    CommandLineParser parser("Header");
    CommandLineParser::Option* debugOption = parser.addFlag({"-d"}, "debug");
    CommandLineParser::Option* infoOption  = parser.addFlag({"-i"}, "info");

    const char *argv[] = {"<bin>"};
    parser.parse(1, argv);
    QCOMPARE(parser.isSet(debugOption), false);
    QCOMPARE(parser.isSet(infoOption), false);
}

void CommandLineParserTest::flagMultiNameTest(){
    CommandLineParser parser("Header");
    CommandLineParser::Option* debugOption = parser.addFlag({"-d"}, "debug");
    CommandLineParser::Option* infoOption  = parser.addFlag({"-i", "--info"}, "info");

    const char *argv[] = {"<bin>", "--info", "-d"};
    parser.parse(3, argv);
    QCOMPARE(parser.isSet(infoOption), true);
    QCOMPARE(parser.isSet(debugOption), true);
}

void CommandLineParserTest::noOptionTest(){
    CommandLineParser parser("Header");
    CommandLineParser::Option* testOption = parser.addOption({"--opt"}, "Test Option", "string");
    const char *argv[] = {"<bin>"};
    parser.parse(1, argv);
    QCOMPARE(parser.isSet(testOption), false);
    QVERIFY_EXCEPTION_THROWN(parser.assertIsSet(testOption), CommandLineParserException);
}

void CommandLineParserTest::optionTest(){
    CommandLineParser parser("Header");
    CommandLineParser::Option* testOption = parser.addOption({"--opt"}, "Test Option", "string");
    const char *argv[] = {"<bin>", "--opt", "test"};
    parser.parse(3, argv);
    QCOMPARE(parser.isSet(testOption), true);
    QVERIFY(parser.value(testOption) == "test");
}

void CommandLineParserTest::optionAndFlagTest(){
    CommandLineParser parser("Header");
    CommandLineParser::Option* testOption = parser.addOption({"--opt"}, "Test Option", "string");
    CommandLineParser::Option* infoOption = parser.addFlag({"--info"}, "Info Option");
    const char *argv[] = {"<bin>", "--opt", "test", "--info"};
    parser.parse(4, argv);
    QCOMPARE(parser.isSet(infoOption), true);
    QCOMPARE(parser.isSet(testOption), true);
    QVERIFY(parser.value(testOption) == "test");
}

void CommandLineParserTest::invalidOptiontest(){
    CommandLineParser parser("Header");
    parser.addOption({"--opt"}, "Test Option", "string");
    parser.addFlag({"--info"}, "Info Option");
    const char *argv[] = {"<bin>", "--opt"};
    QVERIFY_EXCEPTION_THROWN(parser.parse(2, argv), CommandLineParserException);
}


//void CommandLineParserTest::scriptTest(){
//    CommandLineParser parser("Header");
//    parser.addOption("--opt", "Test Option", "string");
//    const char *argv[] = {"<bin>", "--opt", "test", "script_to_run"};
//    parser.parse(4, argv);
//    QCOMPARE(parser.script(), QString("script_to_run"));
//}

//void CommandLineParserTest::scriptNoFlagTest(){
//    CommandLineParser parser("Header");
//    CommandLineParser::Option* debugOption = parser.addScriptFlag("-d", "debug");
//    CommandLineParser::Option* infoOption  = parser.addScriptFlag("-i", "info");

//    const char *argv[] = {"<bin>", "<script>"};
//    parser.parse(2, argv);
//    parser.parseScriptArguments();
//    QCOMPARE(parser.isSet(debugOption), false);
//    QCOMPARE(parser.isSet(infoOption), false);
//}

//void CommandLineParserTest::scriptFlagMultiNameTest(){
//    CommandLineParser parser("Header");
//    CommandLineParser::Option* debugOption = parser.addScriptFlag("-d", "debug");
//    CommandLineParser::Option* infoOption  = parser.addScriptFlag(QStringList() << "-i" << "--info", "info");

//    const char *argv[] = {"<bin>", "<script>", "--info", "-d"};
//    parser.parse(4, argv);
//    parser.parseScriptArguments();
//    QCOMPARE(parser.isSet(infoOption),  true);
//    QCOMPARE(parser.isSet(debugOption), true);
//}

//void CommandLineParserTest::scriptNoOptionTests(){
//    CommandLineParser parser("Header");
//    CommandLineParser::Option* testOption = parser.addScriptOption("--opt", "Test Option", "string");
//    const char *argv[] = {"<bin>", "<script>"};
//    parser.parse(2, argv);
//    QCOMPARE(parser.isSet(testOption), false);
//    QVERIFY_EXCEPTION_THROWN(parser.assertIsSet(testOption), Exception);
//}

//void CommandLineParserTest::scriptOptionTest(){
//    CommandLineParser parser("Header");
//    CommandLineParser::Option* testOption = parser.addScriptOption("--opt", "Test Option", "string");
//    const char *argv[] = {"<bin>", "<script>", "--opt", "test"};
//    parser.parse(4, argv);
//    parser.parseScriptArguments();
//    QCOMPARE(parser.isSet(testOption), true);
//    QCOMPARE(parser.value(testOption), QString("test"));
//}

//void CommandLineParserTest::scriptOptionAndFlagTest(){
//    CommandLineParser parser("Header");
//    CommandLineParser::Option* testOption = parser.addScriptOption("--opt", "Test Option", "string");
//    CommandLineParser::Option* infoOption = parser.addScriptFlag("--info", "Info Option");
//    const char *argv[] = {"<bin>", "<script>", "--opt", "test", "--info"};
//    parser.parse(5, argv);
//    parser.parseScriptArguments();
//    QCOMPARE(parser.isSet(infoOption), true);
//    QCOMPARE(parser.isSet(testOption), true);
//    QCOMPARE(parser.value(testOption), QString("test"));
//}

//void CommandLineParserTest::scriptInvalidOptionTest(){
//    CommandLineParser parser("Header");
//    parser.addScriptOption("--opt", "Test Option", "string");
//    parser.addScriptFlag("--info", "Info Option");
//    const char *argv[] = {"<bin>", "<script>", "--opt"};
//    parser.parse(3, argv);
//    QVERIFY_EXCEPTION_THROWN(parser.parseScriptArguments(), Exception);
//}


