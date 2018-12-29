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

#include <QtGui/QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QFileInfo>

#include "livecv.h"
#include "livecvarguments.h"
#include "live/libraryloadpath.h"
#include "live/visuallog.h"
#include "live/applicationcontext.h"
#include "live/exception.h"

using namespace lv;

int main(int argc, char *argv[]){

    ApplicationContext::initialize();

    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QGuiApplication::addLibraryPath(QString::fromStdString(lv::ApplicationContext::instance().librariesPath()));

    QGuiApplication app(argc, argv);
    QGuiApplication::setApplicationName("Live CV");
    QGuiApplication::setApplicationVersion(LiveCV::versionString());

    try{
        LiveCV::Ptr livecv = LiveCV::create(argc, argv);
        livecv->loadInternals();

        if ( livecv->arguments()->helpFlag() ){
            printf("%s", livecv->arguments()->helpString().c_str());
            return 0;
        } else if ( livecv->arguments()->versionFlag() ){
            printf("%s\n", qPrintable(livecv->versionString()));
            return 0;
        }

        LibraryLoadPath::addRecursive(ApplicationContext::instance().pluginPath(), ApplicationContext::instance().linkPath());
        if ( QFileInfo(QString::fromStdString(ApplicationContext::instance().externalPath())).exists() )
            LibraryLoadPath::addRecursive(ApplicationContext::instance().externalPath(), ApplicationContext::instance().linkPath());

        if ( livecv->arguments()->pluginInfoFlag() ){
            printf("%s", livecv->extractPluginInfo(livecv->arguments()->pluginInfoImport()).data());
            return 0;
        }

        livecv->loadQml(QUrl(QStringLiteral("qrc:/main.qml")));

        return app.exec();

    } catch ( lv::Exception& e ){
        if ( e.code() < 0 ){
            printf("Uncaught exception when initializing: %s\n", e.message().c_str());
            return e.code();
        } else {
            vlog() << "Uncaught exception: " << e.message().c_str();
            return e.code();
        }
    }
}
