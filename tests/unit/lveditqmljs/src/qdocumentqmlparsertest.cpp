/****************************************************************************
**
** Copyright (C) 2014-2018 Dinu SV.
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

#include "qdocumentqmlparsertest.h"
#include "qdocumentqmlparser.h"
#include "qdocumentqmlinfo.h"
#include <QPair>
#include <QCoreApplication>
#include <QtTest/QtTest>

Q_TEST_RUNNER_REGISTER(QDocumentQmlParserTest);

using namespace lcv;

QDocumentQmlParserTest::QDocumentQmlParserTest(QObject *parent)
    : QObject(parent)
{
}

QDocumentQmlParserTest::~QDocumentQmlParserTest(){

}

QString QDocumentQmlParserTest::filePath(const QString &relativePath){
    return QCoreApplication::applicationDirPath() + QDir::separator() + "data" + QDir::separator() + relativePath;
}

QString QDocumentQmlParserTest::readFile(const QString &path){
    QFile file(path);
    if ( !file.open(QIODevice::ReadOnly))
        return "";
    return file.readAll();
}

void QDocumentQmlParserTest::initTestCase(){

}

void QDocumentQmlParserTest::identifierTest(){
    QString file = filePath("mainwindow.in.qml");
    QString code = readFile(file);
    if ( code ==  "" )
        QFAIL("Unable to read input file.");
    QDocumentQmlParser parser;
    QDocumentQmlInfo::MutablePtr doc = parser(file, code);

    QStringList ids = doc->extractIds();
    QCOMPARE(ids.size(), 2);
    QVERIFY(ids.contains("root"));
    QVERIFY(ids.contains("messageDialog"));
}

void QDocumentQmlParserTest::identifierValueTest(){
    QString file = filePath("mainwindow.in.qml");
    QString code = readFile(file);
    if ( code ==  "" )
        QFAIL("Unable to read input file.");
    QDocumentQmlParser parser;
    QDocumentQmlInfo::MutablePtr doc = parser(file, code);

    QVERIFY(!doc->isValueNull(doc->valueForId("messageDialog")));
}

void QDocumentQmlParserTest::identifierFunctionMemberTest(){
    QString file = filePath("mainwindow.in.qml");
    QString code = readFile(file);
    if ( code ==  "" )
        QFAIL("Unable to read input file.");
    QDocumentQmlParser parser;
    QDocumentQmlInfo::MutablePtr doc = parser(file, code);

    QDocumentQmlInfo::ValueReference vref;
    QDocumentQmlObject data = doc->extractValueObject(doc->valueForId("messageDialog"), &vref);
    QCOMPARE(data.typeName(), QString("MessageDialog"));
    QCOMPARE(data.memberProperties().size(), 0);
    QCOMPARE(data.memberSignals().size(), 0);
    QCOMPARE(data.memberSlots().size(), 0);
    QCOMPARE(data.memberFunctions().size(), 1);
    QCOMPARE(data.memberFunctions()["show"].name, QString("show"));
    QCOMPARE(data.memberFunctions()["show"].arguments.size(), 1);
    QCOMPARE(data.memberFunctions()["show"].arguments[0].first, QString("caption"));
}

void QDocumentQmlParserTest::identifierMemberTest(){
    QString file = filePath("propertyexport.in.qml");
    QString code = readFile(file);
    if ( code ==  "" )
        QFAIL("Unable to read input file.");
    QDocumentQmlParser parser;
    QDocumentQmlInfo::MutablePtr doc = parser(file, code);

    QDocumentQmlInfo::ValueReference parentref;
    QDocumentQmlObject data = doc->extractValueObject(doc->valueForId("scope"), &parentref);
    QCOMPARE(data.typeName(), QString("Item"));
    QCOMPARE(data.memberProperties().size(), 1);
    QCOMPARE(data.memberProperties()["scopeVarProperty"], QString("var"));
    QCOMPARE(data.memberSlots().size(), 1);
    QCOMPARE(data.memberSlots()["onScopeVarPropertyChanged"], QString("scopeVarProperty"));

    QDocumentQmlObject rootData = doc->extractValueObject(doc->rootObject(), &parentref);
    QCOMPARE(rootData.typeName(), QString("Item"));

    QCOMPARE(rootData.memberProperties().size(), 3);
    QCOMPARE(rootData.memberProperties()["intProperty"], QString("int"));
    QCOMPARE(rootData.memberProperties()["stringProperty"], QString("string"));
    QCOMPARE(rootData.memberProperties()["rectangleProperty"], QString("Rectangle"));

    QCOMPARE(rootData.memberSignals().size(), 1);
    QCOMPARE(rootData.memberSignals()["exportSignal"].name, QString("exportSignal"));
    QCOMPARE(rootData.memberSignals()["exportSignal"].arguments.size(), 2);
    QCOMPARE(rootData.memberSignals()["exportSignal"].arguments[0].first, QString("stringArgument"));
    QCOMPARE(rootData.memberSignals()["exportSignal"].arguments[1].first, QString("intArgument"));

    QCOMPARE(rootData.memberSlots().size(), 4);
    QCOMPARE(rootData.memberSlots()["onIntPropertyChanged"], QString("intProperty"));
    QCOMPARE(rootData.memberSlots()["onStringPropertyChanged"], QString("stringProperty"));
    QCOMPARE(rootData.memberSlots()["onRectanglePropertyChanged"], QString("rectangleProperty"));
    QCOMPARE(rootData.memberSlots()["onExportSignal"], QString("exportSignal"));
}

void QDocumentQmlParserTest::identifierScopeTest(){
    QString file = filePath("propertyexport.in.qml");
    QString code = readFile(file);
    if ( code ==  "" )
        QFAIL("Unable to read input file.");
    QDocumentQmlParser parser;
    QDocumentQmlInfo::MutablePtr doc = parser(file, code);

    QDocumentQmlInfo::ValueReference parentref;
    QDocumentQmlObject data = doc->extractValueObject(doc->valueForId("scope"), &parentref);
    QVERIFY(!doc->isValueNull(parentref));

    data = doc->extractValueObject(parentref);
    QCOMPARE(data.typeName(), QString("Rectangle"));
    QCOMPARE(data.memberProperties().size(), 1);
    QCOMPARE(data.memberProperties()["parentIntProperty"], QString("int"));
}

void QDocumentQmlParserTest::rangeScopeTest(){
    //HERE
}

