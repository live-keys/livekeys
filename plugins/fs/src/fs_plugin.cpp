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

#include "fs_plugin.h"
#include "listdir.h"
#include "path.h"

#include <qqml.h>
#include <QQmlApplicationEngine>
#include <QQmlEngine>
#include <QQmlContext>
#include <QQuickWindow>


static QObject* pathProvider(QQmlEngine *engine, QJSEngine *){
    return new lv::Path(engine);
}


void FsPlugin::registerTypes(const char *uri){
    // @uri fs
    qmlRegisterType<lv::ListDir>(      uri, 1, 0, "ListDir");
    qmlRegisterSingletonType<lv::Path>(uri, 1, 0, "Path", &pathProvider);
}

void FsPlugin::initializeEngine(QQmlEngine *, const char *){
}
