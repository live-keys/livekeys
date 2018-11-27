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

#include "editqml_plugin.h"

#include <qqml.h>
#include <QQmlApplicationEngine>
#include <QQmlEngine>
#include <QQmlContext>
#include <QQuickWindow>

#include "live/projectqmlextension.h"
#include "live/codeqmlhandler.h"
#include "live/qmladdcontainer.h"
#include "live/qmlpropertymodel.h"
#include "live/qmlitemmodel.h"

void EditQmlPlugin::registerTypes(const char *uri){
    // @uri editqml
    qmlRegisterType<lv::ProjectQmlExtension>(uri, 1, 0, "ProjectQmlExtension");

    qmlRegisterUncreatableType<lv::CodeQmlHandler>(
        uri, 1, 0, "CodeQmlHandler", "CodeQmlHandler can only be accessed through the Editor.documentHandler.");
    qmlRegisterUncreatableType<lv::QmlAddContainer>(
        uri, 1, 0, "QmlAddContainer", "QmlAddContainer can only be accessed through the qmledit extension.");
    qmlRegisterUncreatableType<lv::QmlPropertyModel>(
        uri, 1, 0, "QmlPropertyModel", "QmlPropertyModel can only be accessed through the qmledit extension.");
    qmlRegisterUncreatableType<lv::QmlItemModel>(
        uri, 1, 0, "QmlItemModel", "QmlItemModel can only be accessed through the qmledit extension.");
}

void EditQmlPlugin::initializeEngine(QQmlEngine *, const char *){
}
