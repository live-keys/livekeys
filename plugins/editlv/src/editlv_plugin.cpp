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

#include "editlv_plugin.h"

#include <qqml.h>
#include <QQmlApplicationEngine>
#include <QQmlEngine>
#include <QQmlContext>
#include <QQuickWindow>

#include "languagelvhandler.h"
#include "languagelvextension.h"

void EditLvPlugin::registerTypes(const char *uri){
    // @uri editqml
    qmlRegisterUncreatableType<lv::LanguageLvHandler>(  uri, 1, 0, "LanguageLvHandler", "LanguageLvHandler is created through LanguageLvExtension");
    qmlRegisterType<lv::LanguageLvExtension>(uri, 1, 0, "LanguageLvExtension");
}

void EditLvPlugin::initializeEngine(QQmlEngine *, const char *){
}
