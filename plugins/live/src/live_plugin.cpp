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
#include "qabstractcodeserializer.h"
#include "qnativevaluecodeserializer.h"
#include "qlicensesettings.h"
#include "qcodeconverter.h"
#include "qlivepalette.h"

#include <qqml.h>
#include <QQmlApplicationEngine>
#include <QQmlEngine>
#include <QQmlContext>
#include <QQuickWindow>

void LivePlugin::registerTypes(const char *uri){
    // @uri modules.live
    qmlRegisterType<lcv::QLiveCVMain>(      uri, 1, 0, "Main");
    qmlRegisterType<lcv::QStaticLoader>(    uri, 1, 0, "StaticLoader");
    qmlRegisterUncreatableType<lcv::QLicenseSettings>(
        uri, 1, 0, "LicenseSettings", "LicenseSettings is available through the settings property.");

    qmlRegisterUncreatableType<lcv::QAbstractCodeSerializer>(
        uri, 1, 0, "AbstractCodeSerializer", "Code serializer is of abstract type."
    );
    qmlRegisterType<lcv::QNativeValueCodeSerializer>(uri, 1, 0, "NativeValueCodeSerializer");
    qmlRegisterType<lcv::QCodeConverter>(            uri, 1, 0, "CodeConverter");
    qmlRegisterType<lcv::QLivePalette>(              uri, 1, 0, "LivePalette");
}

void LivePlugin::initializeEngine(QQmlEngine *engine, const char *){
    QStaticContainer* sc = new QStaticContainer(engine);
    engine->rootContext()->setContextProperty("staticContainer", sc);
    lcv::QEngineMonitor* em = new lcv::QEngineMonitor(engine);
    engine->rootContext()->setContextProperty("engineMonitor", em);

    QObject* obj = engine->rootContext()->contextProperty("settings").value<QObject*>();

    QString settingsPath = obj->property("path").toString();
    lcv::QLicenseSettings* ls = new lcv::QLicenseSettings(settingsPath, obj);
    obj->setProperty("license", QVariant::fromValue(ls));
}
