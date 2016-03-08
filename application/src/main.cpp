/****************************************************************************
**
** Copyright (C) 2014 Dinu SV.
** (contact: mail@dinusv.com)
** This file is part of Live CV application.
**
** GNU General Public License Usage
** 
** This file may be used under the terms of the GNU General Public License 
** version 3.0 as published by the Free Software Foundation and appearing 
** in the file LICENSE.GPL included in the packaging of this file.  Please 
** review the following information to ensure the GNU General Public License 
** version 3.0 requirements will be met: http://www.gnu.org/copyleft/gpl.html.
**
****************************************************************************/

#include <QtGui/QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>

#include <QElapsedTimer>

#include "qcodedocument.h"
#include "qcodehandler.h"
#include "qlivecvlog.h"

#include <QQmlEngine>
#include <QDebug>
#include <QDir>
#include <QLibrary>

int main(int argc, char *argv[]){

    QGuiApplication app(argc, argv);
    QGuiApplication::setApplicationName("Live CV");
    QGuiApplication::setApplicationVersion("1.1.0");

    if ( !app.arguments().contains("-c" ) )
        qInstallMessageHandler(&QLiveCVLog::logFunction);

    QQmlApplicationEngine engine;

    QLibrary(QCoreApplication::applicationDirPath() + "/lcvlib").load();

    qmlRegisterType<QCodeDocument>("Cv", 1, 0, "Document");
    qmlRegisterType<QCodeHandler>( "Cv", 1, 0, "CodeHandler");
    qmlRegisterType<QLiveCVLog>(   "Cv", 1, 0, "MessageLog");

    QCodeDocument document;
    engine.rootContext()->setContextProperty("codeDocument", &document);
    engine.rootContext()->setContextProperty("lcvlog", QLiveCVLog::instance());
    engine.load(QUrl(QStringLiteral("plugins/main.qml")));

    document.setEngine(&engine);

    return app.exec();
}
