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

#include "qlivecv.h"
#include "qlivecvarguments.h"

#include <QLibrary>

using namespace lcv;

int main(int argc, char *argv[]){

    QGuiApplication app(argc, argv);
    QGuiApplication::setApplicationName("Live CV");
    QGuiApplication::setApplicationVersion(QLiveCV::versionString());

    QLiveCV::registerTypes();

    QLiveCV livecv(argc, argv);
    if ( livecv.arguments()->helpFlag() ){
        printf("%s", qPrintable(livecv.arguments()->helpString()));
        return 0;
    } else if ( livecv.arguments()->versionFlag() ){
        printf("%s\n", qPrintable(livecv.versionString()));
        return 0;
    }

    livecv.loadLibrary(livecv.dir() + "/lcvlib");

    if ( livecv.arguments()->pluginInfoFlag() ){
        printf("%s", livecv.extractPluginInfo(livecv.arguments()->pluginInfoImport()).data());
        return 0;
    }

    livecv.loadQml(QUrl(QStringLiteral("qrc:/main.qml")));

    return app.exec();
}
