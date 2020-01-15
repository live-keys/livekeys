/****************************************************************************
**
** Copyright (C) 2014-2019 Dinu SV.
** (contact: mail@dinusv.com)
** This file is part of Livekeys Application.
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

#include "livekeys.h"
#include "livekeysarguments.h"
#include "live/libraryloadpath.h"
#include "live/visuallog.h"
#include "live/applicationcontext.h"
#include "live/exception.h"

//#include <QtWebEngine/QtWebEngine>

using namespace lv;

int main(int argc, char *argv[]){

    ApplicationContext::initialize(Livekeys::startupConfiguration());

    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QGuiApplication::addLibraryPath(QString::fromStdString(lv::ApplicationContext::instance().librariesPath()));

    QGuiApplication app(argc, argv);
    QGuiApplication::setApplicationName("Livekeys");
    QGuiApplication::setApplicationVersion(Livekeys::versionString());

//    QtWebEngine::initialize();

    try{
        Livekeys::Ptr livekeys = Livekeys::create(argc, argv);
        livekeys->loadInternals();

        if ( livekeys->arguments()->helpFlag() ){
            printf("%s", livekeys->arguments()->helpString().c_str());
            return 0;
        } else if ( livekeys->arguments()->versionFlag() ){
            printf("%s\n", qPrintable(livekeys->versionString()));
            return 0;
        }

        LibraryLoadPath::addRecursive(ApplicationContext::instance().pluginPath(), ApplicationContext::instance().linkPath());
        if ( QFileInfo(QString::fromStdString(ApplicationContext::instance().externalPath())).exists() )
            LibraryLoadPath::addRecursive(ApplicationContext::instance().externalPath(), ApplicationContext::instance().linkPath());

        return livekeys->exec(app);

    } catch ( lv::Exception& e ){
        if ( e.code() == Exception::toCode("Init") ){
            printf("Uncaught exception when initializing: %s\n", e.message().c_str());
            return e.code();
        } else {
            vlog() << "Uncaught exception: " << e.message().c_str();
            vlog() << *e.stackTrace();
            return e.code();
        }
    }
}
