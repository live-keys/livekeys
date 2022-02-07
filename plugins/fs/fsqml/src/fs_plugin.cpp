/****************************************************************************
**
** Copyright (C) 2022 Dinu SV.
**
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

#include "fs_plugin.h"
#include "listdir.h"
#include "qmlpath.h"
#include "qmldir.h"
#include "qmlfile.h"
#include "qmlfilereader.h"
#include "qmlfiledescriptor.h"
#include "qmlfilestream.h"
#include "qmlurlinfo.h"
#include "qmldirectoryread.h"

#include <qqml.h>
#include <QQmlApplicationEngine>
#include <QQmlEngine>
#include <QQmlContext>
#include <QQuickWindow>

static QObject* pathProvider(QQmlEngine *engine, QJSEngine *){
    return new lv::QmlPath(engine);
}

static QObject* dirProvider(QQmlEngine *engine, QJSEngine *){
    return new lv::QmlDir(engine);
}

static QObject* fileProvider(QQmlEngine *engine, QJSEngine *){
    return new lv::QmlFile(engine);
}

static QObject* urlInfoProvider(QQmlEngine *engine, QJSEngine *){
    return new lv::QmlUrlInfo(engine);
}


void FsPlugin::registerTypes(const char *uri){
    // @uri fs
    qmlRegisterType<lv::ListDir>(            uri, 1, 0, "ListDir");
    qmlRegisterType<lv::QmlFileDescriptor>(  uri, 1, 0, "FileDescriptor");
    qmlRegisterType<lv::QmlFileReader>(      uri, 1, 0, "FileReader");
    qmlRegisterType<lv::QmlFileStream>(      uri, 1, 0, "FileStream");
    qmlRegisterType<lv::QmlDirectoryRead>(   uri, 1, 0, "DirectoryRead");
    qmlRegisterSingletonType<lv::QmlPath>(   uri, 1, 0, "Path", &pathProvider);
    qmlRegisterSingletonType<lv::QmlDir>(    uri, 1, 0, "Dir", &dirProvider);
    qmlRegisterSingletonType<lv::QmlFile>(   uri, 1, 0, "File", &fileProvider);
    qmlRegisterSingletonType<lv::QmlUrlInfo>(uri, 1, 0, "UrlInfo", &urlInfoProvider);
}

void FsPlugin::initializeEngine(QQmlEngine *, const char *){
}
