#include "qplugintypestest.h"
#include "qtypestub.h"

#include <QFile>
#include <QCoreApplication>
#include <QQmlEngine>
#include <QQmlComponent>
#include <QtTest/QtTest>

#include "qplugintypesfacade.h"

Q_TEST_RUNNER_REGISTER(QPluginTypesTest);

using namespace lcv;

QPluginTypesTest::QPluginTypesTest(QObject* parent)
    : QObject(parent)
{
}

QPluginTypesTest::~QPluginTypesTest(){
}

QString QPluginTypesTest::filePath(const QString& relativePath){
    return QCoreApplication::applicationDirPath() + QDir::separator() + "data" + QDir::separator() + relativePath;
}

QByteArray QPluginTypesTest::readFile(const QString& path){
    QFile file(path);
    if ( !file.open(QIODevice::ReadOnly))
        return "";
    return file.readAll();
}

void QPluginTypesTest::initTestCase(){
    qmlRegisterType<QTypeStub>("Stub", 1, 0, "TypeStub");
}

void QPluginTypesTest::getTypesFromEngine(){
    QString file = filePath("customtype.in.qml");
    QByteArray code = readFile(file);
    if ( code ==  "" )
        QFAIL("Unable to read input file.");

    QQmlEngine* engine = new QQmlEngine;
    QQmlComponent* component = new QQmlComponent(engine, QUrl::fromLocalFile(file));
    if (component->isError() )
        QFAIL("Failed to load component: customtype.in.qml");

    component->create();

    QList<const QQmlType*> typeList = QPluginTypesFacade::extractTypes("Stub", engine);
    QCOMPARE(typeList.length(), 1);
    QCOMPARE(QPluginTypesFacade::getTypeName(typeList[0]), QString("TypeStub"));
}

