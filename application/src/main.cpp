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

#include <QThread>

#include "qlivecv.h"

int main(int argc, char *argv[]){

    QLiveCV livecv(argc, argv);
    livecv.loadLibrary(livecv.dir() + "/lcvlib");

    qmlRegisterType<QCodeDocument>("Cv", 1, 0, "Document");
    qmlRegisterType<QCodeHandler>( "Cv", 1, 0, "CodeHandler");
    qmlRegisterType<QLiveCVLog>(   "Cv", 1, 0, "MessageLog");

    livecv.loadQml(QUrl(QStringLiteral("plugins/main.qml")));

    return livecv.exec();
}
