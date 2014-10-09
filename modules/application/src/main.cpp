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
#include <QQmlContext>
#include "qtquick2applicationviewer.h"

#include "QCodeDocument.hpp"
#include "QCodeHandler.hpp"
#include "QCursorShape.hpp"
#include "QLiveCVLog.hpp"

#include <QDebug>
#include <QDir>

int main(int argc, char *argv[]){

    QGuiApplication app(argc, argv);

    qInstallMessageHandler(&QLiveCVLog::logFunction);

    qmlRegisterType<QCursorShape>( "Cv", 1, 0, "CursorArea");
    qmlRegisterType<QCodeDocument>("Cv", 1, 0, "Document");
    qmlRegisterType<QCodeHandler>( "Cv", 1, 0, "CodeHandler");
    qmlRegisterType<QLiveCVLog>(   "Cv", 1, 0, "MessageLog");

    QCodeDocument document;
    QtQuick2ApplicationViewer viewer;
    viewer.rootContext()->setContextProperty("codeDocument", &document);
    viewer.rootContext()->setContextProperty("lcvlog", QLiveCVLog::instance());
    viewer.setMainQmlFile(QStringLiteral("plugins/main.qml"));
    viewer.setTitle("Live CV");
    document.setEngine(viewer.engine());
    viewer.showExpanded();

    return app.exec();
}
