/****************************************************************************
**
** Copyright (C) 2014-2017 Dinu SV.
** (contact: mail@dinusv.com)
** This file is part of Live CV Application.
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

#include "qlivecvcommandlineparsertest.h"
#include "qlivecvcommandlineparser.h"
#include <QtTest/QtTest>

Q_TEST_RUNNER_REGISTER(QLiveCVCommandLineParserTest);

QLiveCVCommandLineParserTest::QLiveCVCommandLineParserTest(QObject *parent)
    : QObject(parent)
{
}

QLiveCVCommandLineParserTest::~QLiveCVCommandLineParserTest(){

}

void QLiveCVCommandLineParserTest::initTestCase(){
}

void QLiveCVCommandLineParserTest::versionFlagTest(){
    QLiveCVCommandLineParser parser("Header");
    const char *argv[] = {"<bin>", "--help"};
    parser.parse(2, argv);
    QCOMPARE(parser.isSet(parser.helpOption()), true);
    QCOMPARE(parser.isSet(parser.versionOption()), false);
}

void QLiveCVCommandLineParserTest::helpFlagTest(){
    QLiveCVCommandLineParser parser("Header");
    const char *argv[] = {"<bin>", "-v"};
    parser.parse(2, argv);
    QCOMPARE(parser.isSet(parser.helpOption()), false);
    QCOMPARE(parser.isSet(parser.versionOption()), true);
}

void QLiveCVCommandLineParserTest::noFlagTest(){
    QLiveCVCommandLineParser parser("Header");
    QLiveCVCommandLineParser::Option* debugOption = parser.addFlag("-d", "debug");
    QLiveCVCommandLineParser::Option* infoOption  = parser.addFlag("-i", "info");

    const char *argv[] = {"<bin>"};
    parser.parse(1, argv);
    QCOMPARE(parser.isSet(debugOption), false);
    QCOMPARE(parser.isSet(infoOption), false);
}

void QLiveCVCommandLineParserTest::flagMultiNameTest(){
    QLiveCVCommandLineParser parser("Header");
    QLiveCVCommandLineParser::Option* debugOption = parser.addFlag("-d", "debug");
    QLiveCVCommandLineParser::Option* infoOption  = parser.addFlag(QStringList() << "-i" << "--info", "info");

    const char *argv[] = {"<bin>", "--info", "-d"};
    parser.parse(3, argv);
    QCOMPARE(parser.isSet(infoOption), true);
    QCOMPARE(parser.isSet(debugOption), true);
}

void QLiveCVCommandLineParserTest::noOptionTest(){
    QLiveCVCommandLineParser parser("Header");
    QLiveCVCommandLineParser::Option* testOption = parser.addOption("--opt", "Test Option", "string");
    const char *argv[] = {"<bin>"};
    parser.parse(1, argv);
    QCOMPARE(parser.isSet(testOption), false);
    QVERIFY_EXCEPTION_THROWN(parser.assertIsSet(testOption), QLiveCVCommandLineException);
}

void QLiveCVCommandLineParserTest::optionTest(){
    QLiveCVCommandLineParser parser("Header");
    QLiveCVCommandLineParser::Option* testOption = parser.addOption("--opt", "Test Option", "string");
    const char *argv[] = {"<bin>", "--opt", "test"};
    parser.parse(3, argv);
    QCOMPARE(parser.isSet(testOption), true);
    QCOMPARE(parser.value(testOption), QString("test"));
}

void QLiveCVCommandLineParserTest::optionAndFlagTest(){
    QLiveCVCommandLineParser parser("Header");
    QLiveCVCommandLineParser::Option* testOption = parser.addOption("--opt", "Test Option", "string");
    QLiveCVCommandLineParser::Option* infoOption = parser.addFlag("--info", "Info Option");
    const char *argv[] = {"<bin>", "--opt", "test", "--info"};
    parser.parse(4, argv);
    QCOMPARE(parser.isSet(infoOption), true);
    QCOMPARE(parser.isSet(testOption), true);
    QCOMPARE(parser.value(testOption), QString("test"));
}

void QLiveCVCommandLineParserTest::invalidOptiontest(){
    QLiveCVCommandLineParser parser("Header");
    parser.addOption("--opt", "Test Option", "string");
    parser.addFlag("--info", "Info Option");
    const char *argv[] = {"<bin>", "--opt"};
    QVERIFY_EXCEPTION_THROWN(parser.parse(2, argv), QLiveCVCommandLineException);
}


void QLiveCVCommandLineParserTest::scriptTest(){
    QLiveCVCommandLineParser parser("Header");
    parser.addOption("--opt", "Test Option", "string");
    const char *argv[] = {"<bin>", "--opt", "test", "script_to_run"};
    parser.parse(4, argv);
    QCOMPARE(parser.script(), QString("script_to_run"));
}

void QLiveCVCommandLineParserTest::scriptNoFlagTest(){
    QLiveCVCommandLineParser parser("Header");
    QLiveCVCommandLineParser::Option* debugOption = parser.addScriptFlag("-d", "debug");
    QLiveCVCommandLineParser::Option* infoOption  = parser.addScriptFlag("-i", "info");

    const char *argv[] = {"<bin>", "<script>"};
    parser.parse(2, argv);
    parser.parseScriptArguments();
    QCOMPARE(parser.isSet(debugOption), false);
    QCOMPARE(parser.isSet(infoOption), false);
}

void QLiveCVCommandLineParserTest::scriptFlagMultiNameTest(){
    QLiveCVCommandLineParser parser("Header");
    QLiveCVCommandLineParser::Option* debugOption = parser.addScriptFlag("-d", "debug");
    QLiveCVCommandLineParser::Option* infoOption  = parser.addScriptFlag(QStringList() << "-i" << "--info", "info");

    const char *argv[] = {"<bin>", "<script>", "--info", "-d"};
    parser.parse(4, argv);
    parser.parseScriptArguments();
    QCOMPARE(parser.isSet(infoOption),  true);
    QCOMPARE(parser.isSet(debugOption), true);
}

void QLiveCVCommandLineParserTest::scriptNoOptionTests(){
    QLiveCVCommandLineParser parser("Header");
    QLiveCVCommandLineParser::Option* testOption = parser.addScriptOption("--opt", "Test Option", "string");
    const char *argv[] = {"<bin>", "<script>"};
    parser.parse(2, argv);
    QCOMPARE(parser.isSet(testOption), false);
    QVERIFY_EXCEPTION_THROWN(parser.assertIsSet(testOption), QLiveCVCommandLineException);
}

void QLiveCVCommandLineParserTest::scriptOptionTest(){
    QLiveCVCommandLineParser parser("Header");
    QLiveCVCommandLineParser::Option* testOption = parser.addScriptOption("--opt", "Test Option", "string");
    const char *argv[] = {"<bin>", "<script>", "--opt", "test"};
    parser.parse(4, argv);
    parser.parseScriptArguments();
    QCOMPARE(parser.isSet(testOption), true);
    QCOMPARE(parser.value(testOption), QString("test"));
}

void QLiveCVCommandLineParserTest::scriptOptionAndFlagTest(){
    QLiveCVCommandLineParser parser("Header");
    QLiveCVCommandLineParser::Option* testOption = parser.addScriptOption("--opt", "Test Option", "string");
    QLiveCVCommandLineParser::Option* infoOption = parser.addScriptFlag("--info", "Info Option");
    const char *argv[] = {"<bin>", "<script>", "--opt", "test", "--info"};
    parser.parse(5, argv);
    parser.parseScriptArguments();
    QCOMPARE(parser.isSet(infoOption), true);
    QCOMPARE(parser.isSet(testOption), true);
    QCOMPARE(parser.value(testOption), QString("test"));
}

void QLiveCVCommandLineParserTest::scriptInvalidOptionTest(){
    QLiveCVCommandLineParser parser("Header");
    parser.addScriptOption("--opt", "Test Option", "string");
    parser.addScriptFlag("--info", "Info Option");
    const char *argv[] = {"<bin>", "<script>", "--opt"};
    parser.parse(3, argv);
    QVERIFY_EXCEPTION_THROWN(parser.parseScriptArguments(), QLiveCVCommandLineException);
}


