/****************************************************************************
**
** Copyright (C) 2014-2016 Dinu SV.
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

#include <QtGui/QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>

#include "qcodedocument.h"
#include "qcodehandler.h"
#include "qlivecvlog.h"
#include "qlivecv.h"

#include <QLibrary>


int main(int argc, char *argv[]){

    QGuiApplication app(argc, argv);
    QGuiApplication::setApplicationName("Live CV");
    QGuiApplication::setApplicationVersion(QLiveCV::versionString());

    QLiveCV livecv(app.arguments());
    livecv.loadLibrary(livecv.dir() + "/lcvlib");

    qmlRegisterUncreatableType<QCodeDocument>("Cv", 1, 0, "Document", "Only access to the document object is allowed.");
    qmlRegisterUncreatableType<QLiveCVLog>(   "Cv", 1, 0, "MessageLog", "Type is singleton.");
    qmlRegisterType<QCodeHandler>(            "Cv", 1, 0, "CodeHandler");

    livecv.loadQml(QUrl(QStringLiteral("qrc:/main.qml")));

    return app.exec();
}
