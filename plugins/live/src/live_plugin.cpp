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

#include "live_plugin.h"
#include "qmlmain.h"
#include "qstaticloader.h"
#include "qstaticcontainer.h"
#include "qlicensesettings.h"
#include "qfilereader.h"
#include "qstaticfilereader.h"
#include "qtriangle.h"
#include "qloglistener.h"
#include "worker.h"
#include "tuple.h"
#include "qvaluehistory.h"
#include "stringbasedloader.h"
#include "live/qmlobjectlist.h"
#include "live/qmlvariantlist.h"

#include "qmlfork.h"
#include "qmlforknode.h"
#include "remoteline.h"
#include "remotecontainer.h"
#include "qmlcomponentmap.h"
#include "qmlcomponentmapdata.h"

#include "live/applicationcontext.h"
#include "live/visuallog.h"
#include "live/settings.h"
#include "live/visuallogfilter.h"

#include "qmlsubproject.h"
#include "componentsource.h"
#include "tcplineconnection.h"
#include "remotelineresponse.h"
#include "tcplineserver.h"

#include <qqml.h>
#include <QQmlApplicationEngine>
#include <QQmlEngine>
#include <QQmlContext>
#include <QQuickWindow>
#include "qmlcolor.h"

static QObject* colorProvider(QQmlEngine *engine, QJSEngine *){
    return new lv::QmlColor(engine);
}


void LivePlugin::registerTypes(const char *uri){
    // @uri modules.live
    qmlRegisterType<QTriangle>(            uri, 1, 0, "Triangle");
    qmlRegisterType<QStaticLoader>(        uri, 1, 0, "StaticLoader");
    qmlRegisterType<QFileReader>(          uri, 1, 0, "FileReader");
    qmlRegisterType<QStaticFileReader>(    uri, 1, 0, "StaticFileReader");
    qmlRegisterType<lv::VisualLogFilter>(  uri, 1, 0, "VisualLogFilter");
    qmlRegisterType<QLogListener>(         uri, 1, 0, "LogListener");
    qmlRegisterType<QValueHistory>(        uri, 1, 0, "ValueHistory");
    qmlRegisterType<lv::QmlMain>(          uri, 1, 0, "Main");
    qmlRegisterType<lv::StringBasedLoader>(uri, 1, 0, "StringBasedLoader");
    qmlRegisterType<lv::Worker>(           uri, 1, 0, "Worker");
    qmlRegisterType<lv::Tuple>(            uri, 1, 0, "Tuple");
    qmlRegisterType<lv::QmlFork>(          uri, 1, 0, "Fork");
    qmlRegisterType<lv::QmlForkNode>(      uri, 1, 0, "ForkNode");
    qmlRegisterType<lv::ComponentSource>(  uri, 1, 0, "ComponentSource");
    qmlRegisterType<lv::RemoteLine>(       uri, 1, 0, "RemoteLine");
    qmlRegisterType<lv::QmlComponentMap>(     uri, 1, 0, "ComponentMap");
    qmlRegisterType<lv::QmlComponentMapData>( uri, 1, 0, "ComponentMapData");

    qmlRegisterUncreatableType<QLicenseSettings>(
        uri, 1, 0, "LicenseSettings", "LicenseSettings is available through the settings property.");

    qmlRegisterType<lv::TcpLineServer>(         uri, 1, 0, "TcpLineServer");
    qmlRegisterType<lv::QmlSubproject>(         uri, 1, 0, "Subproject");
    qmlRegisterType<lv::TcpLineConnection>(     uri, 1, 0, "TcpLineConnection");
    qmlRegisterUncreatableType<lv::RemoteLineResponse>(
        uri, 1, 0, "RemoteLineResponse", "RemoteLineResponse is part of RemoteLine.");
    qmlRegisterUncreatableType<lv::RemoteContainer>(
        uri, 1, 0, "RemoteContainer", "RemoteContainer is of abstract type.");

    qmlRegisterSingletonType<lv::QmlColor>(uri, 1, 0, "Color", &colorProvider);
}

void LivePlugin::initializeEngine(QQmlEngine *engine, const char *){
    QStaticContainer* sc = new QStaticContainer(engine);
    engine->rootContext()->setContextProperty("staticContainer", sc);

    QObject* livecv   = engine->rootContext()->contextProperty("livecv").value<QObject*>();
    lv::Settings* settings = static_cast<lv::Settings*>(livecv->property("settings").value<QObject*>());

    QString settingsPath = settings->path();
    QLicenseSettings* ls = new QLicenseSettings(settingsPath, settings);
    settings->addConfigFile("license", ls);
}
