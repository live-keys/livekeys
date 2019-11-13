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

#include "editjson_plugin.h"

#include <qqml.h>
#include <QQmlApplicationEngine>
#include <QQmlEngine>
#include <QQmlContext>
#include <QQuickWindow>

#include "editjsonobject.h"
#include "qmllanguageobject.h"
#include "qmlscriptlanguagehandler.h"

static QObject* editJsonObjectProvider(QQmlEngine *engine, QJSEngine *){
    return new lv::EditJsonObject(engine);
}

void EditJsonPlugin::registerTypes(const char *uri){
    // @uri editqml
    qmlRegisterUncreatableType<lv::QmlScriptLanguageHandler>(
        uri, 1, 0, "ScriptLanguageHandler",
        "ScriptLanguageHandler can only be created using the EditJsonObject.");
    qmlRegisterSingletonType<lv::EditJsonObject>(uri, 1, 0, "EditJsonObject",        &editJsonObjectProvider);
    qmlRegisterUncreatableType<lv::QmlLanguageObject>(
        uri, 1, 0, "LanguageObject",
        "LanguageObject can only be created by language providers.");
}

void EditJsonPlugin::initializeEngine(QQmlEngine *, const char *){
}
