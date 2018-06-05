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
#include "qlivecvmain.h"
#include "qstaticloader.h"
#include "qenginemonitor.h"
#include "qstaticcontainer.h"
#include "qlicensesettings.h"
#include "qfilereader.h"
#include "qstaticfilereader.h"
#include "qtriangle.h"
#include "qloglistener.h"
#include "componentsource.h"
#include "thread.h"
#include "tuple.h"
#include "tcpline.h"
#include "tcpagent.h"
#include "qvaluehistory.h"
#include "stringbasedloader.h"
#include "qmlobjectlist.h"
#include "qmlvariantlist.h"

#include "live/plugincontext.h"
#include "live/visuallog.h"
#include "live/settings.h"
#include "live/visuallogfilter.h"

#include <qqml.h>
#include <QQmlApplicationEngine>
#include <QQmlEngine>
#include <QQmlContext>
#include <QQuickWindow>

void LivePlugin::registerTypes(const char *uri){
    // @uri modules.live
    qmlRegisterType<QLiveCVMain>(          uri, 1, 0, "Main");
    qmlRegisterType<QTriangle>(            uri, 1, 0, "Triangle");
    qmlRegisterType<QStaticLoader>(        uri, 1, 0, "StaticLoader");
    qmlRegisterType<QFileReader>(          uri, 1, 0, "FileReader");
    qmlRegisterType<QStaticFileReader>(    uri, 1, 0, "StaticFileReader");
    qmlRegisterType<lv::VisualLogFilter>(  uri, 1, 0, "VisualLogFilter");
    qmlRegisterType<QLogListener>(         uri, 1, 0, "LogListener");
    qmlRegisterType<QValueHistory>(        uri, 1, 0, "ValueHistory");
    qmlRegisterUncreatableType<QLicenseSettings>(
        uri, 1, 0, "LicenseSettings", "LicenseSettings is available through the settings property.");
    qmlRegisterType<lv::StringBasedLoader>(uri, 1, 0, "StringBasedLoader");
    qmlRegisterType<lv::ComponentSource>(  uri, 1, 0, "ComponentSource");
    qmlRegisterType<lv::Thread>(           uri, 1, 0, "Thread");
    qmlRegisterType<lv::Tuple>(            uri, 1, 0, "Tuple");
    qmlRegisterType<lv::TcpLine>(          uri, 1, 0, "TcpLine");
    qmlRegisterType<lv::TcpAgent>(         uri, 1, 0, "TcpAgent");
    qmlRegisterUncreatableType<lv::QmlObjectList>(
        uri, 1, 0, "QmlObjectList", "QmlObjectList can only be created internally.");
    qmlRegisterUncreatableType<lv::QmlVariantList>(
        uri, 1, 0, "QmlVariantList", "QmlVariantList can only be created internally.");
}

void LivePlugin::initializeEngine(QQmlEngine *engine, const char *){
    QStaticContainer* sc = new QStaticContainer(engine);
    engine->rootContext()->setContextProperty("staticContainer", sc);
    QEngineMonitor* em = new QEngineMonitor(engine);
    engine->rootContext()->setContextProperty("engineMonitor", em); //TODO: Remove

    QObject* livecv   = engine->rootContext()->contextProperty("livecv").value<QObject*>();
    lv::Settings* settings = static_cast<lv::Settings*>(livecv->property("settings").value<QObject*>());

    QString settingsPath = settings->path();
    QLicenseSettings* ls = new QLicenseSettings(settingsPath, settings);
    settings->addConfigFile("license", ls);
}
