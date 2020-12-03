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

#include "editqml_plugin.h"

#include <qqml.h>
#include <QQmlApplicationEngine>
#include <QQmlEngine>
#include <QQmlContext>
#include <QQuickWindow>

#include "live/projectqmlextension.h"
#include "live/qmlimportsmodel.h"
#include "qmlplugininfoextractor.h"
#include "qmldocumentsyntax.h"
#include "qmldocumentsyntaxvalue.h"

void EditQmlPlugin::registerTypes(const char *uri){
    // @uri editqml
    lv::ProjectQmlExtension::registerTypes(uri);
    qmlRegisterType<lv::QmlPluginInfoExtractor>(uri, 1, 0, "PluginInfoExtractor");
    qmlRegisterType<lv::QmlDocumentSyntax>(     uri, 1, 0, "QmlDocumentSyntax");

    qmlRegisterUncreatableType<lv::QmlImportsModel>(
        uri, 1, 0, "QmlImportsModel", "\'QmlImportsModel\' is created from a CodeQmlHandler object.");
    qmlRegisterUncreatableType<lv::QmlDocumentSyntaxValue>(
        uri, 1, 0, "QmlDocumentSyntaxValue", "\'QmlDocumentSyntaxValue\' is available through the QmlDocumentSyntax object.");
}

void EditQmlPlugin::initializeEngine(QQmlEngine *, const char *){
}
