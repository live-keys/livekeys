/****************************************************************************
**
** Copyright (C) 2014-2017 Dinu SV.
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

#include "live/abstractcodeserializer.h"
#include "live/nativevaluecodeserializer.h"
#include "live/qmlobjectcodeserializer.h"
#include "live/codeconverter.h"
#include "live/livepalette.h"
#include "live/plugincontext.h"
#include "live/visuallog.h"
#include "live/settings.h"

#include <qqml.h>
#include <QQmlApplicationEngine>
#include <QQmlEngine>
#include <QQmlContext>
#include <QQuickWindow>

void LivePlugin::registerTypes(const char *uri){
    // @uri modules.live
    qmlRegisterType<QLiveCVMain>(      uri, 1, 0, "Main");
    qmlRegisterType<QStaticLoader>(    uri, 1, 0, "StaticLoader");
    qmlRegisterType<QFileReader>(      uri, 1, 0, "FileReader");
    qmlRegisterType<QStaticFileReader>(uri, 1, 0, "StaticFileReader");
    qmlRegisterUncreatableType<QLicenseSettings>(
        uri, 1, 0, "LicenseSettings", "LicenseSettings is available through the settings property.");

    qmlRegisterUncreatableType<lv::AbstractCodeSerializer>(
        uri, 1, 0, "AbstractCodeSerializer", "Code serializer is of abstract type."
    );
    qmlRegisterType<lv::NativeValueCodeSerializer>(uri, 1, 0, "NativeValueCodeSerializer");
    qmlRegisterType<lv::QmlObjectCodeSerializer>(  uri, 1, 0, "QmlObjectCodeSerializer");
    qmlRegisterType<lv::CodeConverter>(            uri, 1, 0, "CodeConverter");
    qmlRegisterType<lv::LivePalette>(              uri, 1, 0, "LivePalette");
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
