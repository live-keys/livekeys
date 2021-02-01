#include <QGuiApplication>
#include <QQmlApplicationEngine>

#include "fileio.h"
#include "jsmodule.h"

#include <QQuickItem>
#include <QQmlComponent>
#include <QQmlEngine>
#include <QQmlContext>

void registerTypes(){
    qmlRegisterSingletonType<FileIO>("paper", 1, 0, "FileIO", &FileIO::provider);
    qmlRegisterSingletonType<JsModule>("paper", 1, 0, "JsModule", &JsModule::provider);
}

int main(int argc, char *argv[]){

    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QGuiApplication app(argc, argv);

    QQmlApplicationEngine engine;

#ifdef Q_OS_WIN
    engine.rootContext()->setContextProperty("os_win", true);
#endif

    registerTypes();

    const QUrl url(QStringLiteral("qrc:/main.qml"));
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated, &app, [url](QObject *obj, const QUrl &objUrl) {
        if (!obj && url == objUrl)
            QCoreApplication::exit(-1);
    }, Qt::QueuedConnection);
    engine.load(url);

    return app.exec();
}
