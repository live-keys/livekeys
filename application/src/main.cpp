/****************************************************************************
**
** Copyright (C) 2022 Dinu SV.
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

#include <QtWebEngine/QtWebEngine>

using namespace lv;

int main(int argc, char *argv[]){

    ApplicationContext::initialize(Livekeys::startupConfiguration());

#if (QT_VERSION_MAJOR < 6)
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
#endif

    QGuiApplication::addLibraryPath(QString::fromStdString(lv::ApplicationContext::instance().librariesPath()));

#if (QT_VERSION >= QT_VERSION_CHECK(5,12,0))
    QtWebEngine::initialize();
#endif
    
    QGuiApplication app(argc, argv);
    QGuiApplication::setApplicationName("Livekeys");
    QGuiApplication::setApplicationVersion(Livekeys::versionString());

#if (QT_VERSION < QT_VERSION_CHECK(5,12,0))
    QtWebEngine::initialize();
#endif

    app.setOrganizationName("Livekeys");
    app.setOrganizationDomain("Livekeys");

    try{
        Livekeys::Ptr livekeys = Livekeys::create(argc, argv);
        livekeys->loadInternals();

        return livekeys->run(app);

    } catch ( lv::Exception& e ){
        if ( e.code() == Exception::toCode("Init") ){
            printf("Uncaught exception when initializing: %s\n", e.message().c_str());
            return e.code();
        } else {
            vlog("main").e() << "Uncaught exception: " << e.message().c_str();
            vlog("main").e() << *e.stackTrace();
            return e.code();
        }
    }
}
