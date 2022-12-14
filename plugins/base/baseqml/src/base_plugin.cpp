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

#include "base_plugin.h"
#include "live/viewengine.h"
#include "live/project.h"
#include "live/viewcontext.h"
#include "live/applicationcontext.h"
#include "live/visuallogfilter.h"
#include "qmlscript.h"
#include "qmlpropertylog.h"
#include "qmlindexselector.h"
#include "environment.h"
#include "qmlexec.h"
#include "qmlstreamlog.h"
#include "qmlstreamsink.h"
#include "qmlworkerpoolobject.h"
#include "qmlcontainer.h"
#include "qmlstreamfilter.h"
#include "qmlstreamvalue.h"
#include "qmlstreamvalueawait.h"
#include "qmlstreamact.h"
#include "qmlthreadinfo.h"
#include "qmltime.h"
#include "qmlmain.h"
#include "qmldata.h"
#include "qmlarrange.h"
#include "qmlcollector.h"
#include "qmlsplit.h"
#include "qmlstreamoperator.h"
#include "qmlvalueflow.h"
#include "qmlvalueflowsink.h"
#include "qmlvalueflowinput.h"
#include "qmlstreamvalueflow.h"
#include "qmlvalueflowgraph.h"
#include "qmlmap.h"
#include "qmlstreamproviderscript.h"
#include "qmlstreamoperations.h"
#include "qmlpromiseop.h"

#include <qqml.h>
#include <QQmlApplicationEngine>
#include <QQmlEngine>
#include <QQmlContext>
#include <QQuickWindow>

static QObject* workerPoolProvider(QQmlEngine* engine, QJSEngine*){
    return new lv::QmlWorkerPoolObject(engine);
}

static QObject* threadInfoProvider(QQmlEngine* engine, QJSEngine*){
    return new lv::QmlThreadInfo(engine);
}

static QObject* timeProvider(QQmlEngine* engine, QJSEngine*){
    return new lv::QmlTime(engine);
}

static QObject* scriptProvider(QQmlEngine *engine, QJSEngine *){
    return new lv::QmlScript(engine, lv::ApplicationContext::instance().scriptArguments());;
}

static QObject* streamOperationsProvider(QQmlEngine *engine, QJSEngine *){
    return new lv::QmlStreamOperations(engine);
}

static QObject* promiseOpProvider(QQmlEngine *engine, QJSEngine *){
    return new lv::QmlPromiseOp(engine);
}

void BasePlugin::registerTypes(const char *uri){
    lv::ViewEngine::registerBaseTypes(uri);
    qmlRegisterType<lv::QmlIndexSelector>(         uri, 1, 0, "IndexSelector");
    qmlRegisterType<lv::QmlMain>(                  uri, 1, 0, "Main");
    qmlRegisterType<lv::QmlExec>(                  uri, 1, 0, "Exec");
    qmlRegisterType<lv::QmlStreamLog>(             uri, 1, 0, "StreamLog");
    qmlRegisterType<lv::QmlPropertyLog>(           uri, 1, 0, "PropertyLog");
    qmlRegisterType<lv::QmlContainer>(             uri, 1, 0, "Container");
    qmlRegisterType<lv::QmlArrange>(               uri, 1, 0, "Arrange");
    qmlRegisterType<lv::QmlSplit>(                 uri, 1, 0, "Split");
    qmlRegisterType<lv::QmlCollector>(             uri, 1, 0, "Collector");
    qmlRegisterType<lv::QmlData>(                  uri, 1, 0, "Data");
    qmlRegisterType<lv::QmlStreamFilter>(          uri, 1, 0, "StreamFilter");
    qmlRegisterType<lv::QmlStreamSink>(            uri, 1, 0, "StreamSink");
    qmlRegisterType<lv::QmlStreamValue>(           uri, 1, 0, "StreamValue");
    qmlRegisterType<lv::QmlStreamValueAwait>(      uri, 1, 0, "StreamValueAwait");
    qmlRegisterType<lv::QmlStreamAct>(             uri, 1, 0, "StreamAct");
    qmlRegisterType<lv::QmlValueFlow>(             uri, 1, 0, "ValueFlow");
    qmlRegisterType<lv::QmlValueFlowInput>(        uri, 1, 0, "ValueFlowInput");
    qmlRegisterType<lv::QmlValueFlowSink>(         uri, 1, 0, "ValueFlowSink");
    qmlRegisterType<lv::QmlStreamValueFlow>(       uri, 1, 0, "StreamValueFlow");
    qmlRegisterType<lv::QmlMap>(                   uri, 1, 0, "Map");
    qmlRegisterType<lv::QmlValueFlowGraph>(        uri, 1, 0, "ValueFlowGraph");
    qmlRegisterType<lv::VisualLogFilter>(          uri, 1, 0, "VisualLogFilter");

    lv::ViewEngine::registerPropertyParserType<lv::QmlAct>(
        uri, 1, 0, "Act", &lv::QmlAct::parserPropertyValidateHook, &lv::QmlAct::parserPropertyHook);

    qmlRegisterSingletonType<lv::QmlStreamOperations>(      uri, 1, 0, "StreamOperations", &streamOperationsProvider);
    qmlRegisterSingletonType<lv::QmlScript>(                uri, 1, 0, "Script", &scriptProvider);
    qmlRegisterSingletonType<lv::QmlWorkerPoolObject>(      uri, 1, 0, "WorkerPool", &workerPoolProvider);
    qmlRegisterSingletonType<lv::QmlTime>(                  uri, 1, 0, "Time", &timeProvider);
    qmlRegisterSingletonType<lv::QmlThreadInfo>(            uri, 1, 0, "ThreadInfo", &threadInfoProvider);
    qmlRegisterSingletonType<lv::QmlPromiseOp>(             uri, 1, 0, "PromiseOp", &promiseOpProvider);

    qmlRegisterUncreatableType<lv::QmlStreamProviderScript>(
        uri, 1, 0, "StreamProviderScript", "StreamProviderScript can be created from StreamOperations.");
    qmlRegisterUncreatableType<lv::QmlStreamOperator>(
        uri, 1, 0, "StreamOperator", "StreamOperator can be created from StreamOperations.");
    qmlRegisterUncreatableType<lv::Environment>(
        uri, 1, 0, "Environment", "Use 'base.Script.environment' to access the environment property.");
    qmlRegisterUncreatableType<lv::QmlWorkerInterface>(
        uri, 1, 0, "WorkerInterface", "WorkerInterface is of abstract type.");
}

void BasePlugin::initializeEngine(QQmlEngine *engine, const char *){
    lv::ViewContext::initFromEngine(engine);
}
