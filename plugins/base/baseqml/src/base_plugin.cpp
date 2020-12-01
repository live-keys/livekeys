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

#include "base_plugin.h"
#include "live/viewengine.h"
#include "live/project.h"
#include "live/viewcontext.h"
#include "live/applicationcontext.h"
#include "qmlscript.h"
#include "qmlpropertylog.h"
#include "qmlindexselector.h"
#include "environment.h"
#include "qmlexec.h"
#include "qmlstreamlog.h"
#include "qmlworkerpoolobject.h"
#include "qmlcontainer.h"
#include "qmlopening.h"
#include "qmlfollowup.h"
#include "groupcollector.h"
#include "qmlstreamfilter.h"

#include <qqml.h>
#include <QQmlApplicationEngine>
#include <QQmlEngine>
#include <QQmlContext>
#include <QQuickWindow>

static QObject* workerPoolProvider(QQmlEngine* engine, QJSEngine*){
    return new lv::QmlWorkerPoolObject(engine);
}

static QObject* scriptProvider(QQmlEngine *engine, QJSEngine *){
    QObject* projectOb = engine->rootContext()->contextProperty("project").value<QObject*>();
    if ( !projectOb )
        THROW_EXCEPTION(lv::Exception, "Failed to load project context property.", 1);

    lv::Project* project = static_cast<lv::Project*>(projectOb);
    lv::QmlScript* script = new lv::QmlScript(engine, lv::ApplicationContext::instance().scriptArguments());
    QObject::connect(project, &lv::Project::activeChanged, script, &lv::QmlScript::scriptChanged);

    return script;
}

void BasePlugin::registerTypes(const char *uri){
    lv::ViewEngine::registerBaseTypes(uri);
    qmlRegisterType<lv::QmlIndexSelector>(uri, 1, 0, "IndexSelector");
    qmlRegisterType<lv::QmlAct>(                uri, 1, 0, "Act");
    qmlRegisterType<lv::QmlExec>(         uri, 1, 0, "Exec");
    qmlRegisterType<lv::QmlStreamLog>(    uri, 1, 0, "StreamLog");
    qmlRegisterType<lv::QmlPropertyLog>(  uri, 1, 0, "PropertyLog");
    qmlRegisterType<lv::QmlContainer>(    uri, 1, 0, "Container");
    qmlRegisterType<lv::QmlOpening>(      uri, 1, 0, "Opening");
    qmlRegisterType<lv::QmlFollowUp>(     uri, 1, 0, "FollowUp");
    qmlRegisterType<lv::GroupCollector>(  uri, 1, 0, "GroupCollector");
    qmlRegisterType<lv::QmlStreamFilter>( uri, 1, 0, "StreamFilter");

    qmlRegisterSingletonType<lv::QmlScript>(           uri, 1, 0, "Script", &scriptProvider);
    qmlRegisterSingletonType<lv::QmlWorkerPoolObject>( uri, 1, 0, "WorkerPool", &workerPoolProvider);

    qmlRegisterUncreatableType<lv::Environment>(
        uri, 1, 0, "Environment", "Use 'base.Script.environment' to access the environment property.");
    qmlRegisterUncreatableType<lv::QmlWorkerInterface>(
        uri, 1, 0, "WorkerInterface", "WorkerInterface is of abstract type.");
}

void BasePlugin::initializeEngine(QQmlEngine *engine, const char *){
    lv::ViewContext::initFromEngine(engine);
}
