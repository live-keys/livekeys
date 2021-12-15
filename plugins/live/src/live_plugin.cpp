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

#include "live_plugin.h"
#include "staticloader.h"
#include "live/staticcontainer.h"
#include "licensesettings.h"
#include "loglistener.h"
#include "worker.h"
#include "valuehistory.h"
#include "stringbasedloader.h"
#include "live/qmlobjectlist.h"
#include "live/qmlvariantlist.h"
#include "eventrelay.h"

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

static QObject* eventRelayProvider(QQmlEngine *engine, QJSEngine *){
    return new lv::EventRelay(engine);
}

void LivePlugin::registerTypes(const char *uri){
    // @uri modules.live
    qmlRegisterType<lv::StaticLoader>(        uri, 1, 0, "StaticLoader");
    qmlRegisterType<lv::VisualLogFilter>(     uri, 1, 0, "VisualLogFilter");
    qmlRegisterType<lv::LogListener>(         uri, 1, 0, "LogListener");
    qmlRegisterType<lv::ValueHistory>(        uri, 1, 0, "ValueHistory");
    qmlRegisterType<lv::StringBasedLoader>(   uri, 1, 0, "StringBasedLoader");
    qmlRegisterType<lv::Worker>(              uri, 1, 0, "Worker");
    qmlRegisterType<lv::QmlFork>(             uri, 1, 0, "Fork");
    qmlRegisterType<lv::QmlForkNode>(         uri, 1, 0, "ForkNode");
    qmlRegisterType<lv::ComponentSource>(     uri, 1, 0, "ComponentSource");
    qmlRegisterType<lv::RemoteLine>(          uri, 1, 0, "RemoteLine");
    qmlRegisterType<lv::QmlComponentMap>(     uri, 1, 0, "ComponentMap");
    qmlRegisterType<lv::QmlComponentMapData>( uri, 1, 0, "ComponentMapData");

    qmlRegisterUncreatableType<lv::LicenseSettings>(
        uri, 1, 0, "LicenseSettings", "LicenseSettings is available through the settings property.");

    qmlRegisterType<lv::TcpLineServer>(         uri, 1, 0, "TcpLineServer");
    qmlRegisterType<lv::QmlSubproject>(         uri, 1, 0, "Subproject");
    qmlRegisterType<lv::TcpLineConnection>(     uri, 1, 0, "TcpLineConnection");
    qmlRegisterUncreatableType<lv::RemoteLineResponse>(
        uri, 1, 0, "RemoteLineResponse", "RemoteLineResponse is part of RemoteLine.");
    qmlRegisterUncreatableType<lv::RemoteContainer>(
        uri, 1, 0, "RemoteContainer", "RemoteContainer is of abstract type.");

    qmlRegisterSingletonType<lv::QmlColor>(uri, 1, 0, "Color", &colorProvider);
    qmlRegisterSingletonType<lv::EventRelay>(uri, 1, 0, "EventRelay", &eventRelayProvider);
}

void LivePlugin::initializeEngine(QQmlEngine *engine, const char *){
    lv::StaticContainer* sc = new lv::StaticContainer(engine);
    engine->rootContext()->setContextProperty("staticContainer", sc);

    QObject* livekeys   = engine->rootContext()->contextProperty("lk").value<QObject*>();
    lv::Settings* settings = static_cast<lv::Settings*>(livekeys->property("settings").value<QObject*>());

    QString settingsPath = settings->path();
    lv::LicenseSettings* ls = new lv::LicenseSettings(settingsPath, settings);
    settings->addConfigFile("license", ls);
}
