#include "qdocumentqmlparsertest.h"
#include "qdocumentqmlparser.h"
#include "qdocumentqmlinfo.h"
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

    QVERIFY(doc->valueForId("messageDialog") != 0);
}

void QDocumentQmlParserTest::identifierMemberTest(){
    QString file = filePath("mainwindow.in.qml");
    QString code = readFile(file);
    if ( code ==  "" )
        QFAIL("Unable to read input file.");
    QDocumentQmlParser parser;
    QDocumentQmlInfo::MutablePtr doc = parser(file, code);

//    doc->extractValueData(doc->valueForId("messageDialog"));
    doc->extractValueData(doc->valueForId("root"));
}

