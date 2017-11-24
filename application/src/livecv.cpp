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

#include "livecv.h"
#include "livecvarguments.h"
#include "livecvscript.h"
#include "commands.h"
#include "environment.h"

#include "live/keymap.h"
#include "live/visuallog.h"
#include "live/visuallogqt.h"
#include "live/visuallogmodel.h"
#include "live/visuallognetworksender.h"
#include "live/visuallogjsobject.h"
#include "live/engine.h"
#include "live/errorhandler.h"
#include "live/settings.h"
#include "live/plugincontext.h"

#include "live/editor_plugin.h"
#include "live/project.h"
#include "live/documenthandler.h"

#include "live/projectqmlextension.h"
#include "live/qmljssettings.h"
#include "live/codeqmlhandler.h"
#include "live/documentqmlinfo.h"
#include "live/plugininfoextractor.h"

#include <QUrl>
#include <QFileInfo>
#include <QDir>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QQuickWindow>
#include <QGuiApplication>


namespace lv{

LiveCV::LiveCV(QObject *parent)
    : QObject(parent)
    , m_engine(new Engine(new QQmlApplicationEngine))
    , m_arguments(new LiveCVArguments(header()))
    , m_codeInterface(0)
    , m_dir(PluginContext::applicationPath())
    , m_project(new Project)
    , m_settings(0)
    , m_script(0)
    , m_commands(new Commands)
    , m_keymap(0)
    , m_log(new VisualLogModel(m_engine->engine()))
    , m_vlog(new VisualLogJsObject) // js ownership
{
    VisualLog::setViewTransport(m_log);
}

LiveCV::~LiveCV(){
    delete m_settings;
    delete m_engine;
}

LiveCV::Ptr LiveCV::create(int argc, const char * const argv[], QObject *parent){
    LiveCV::Ptr livecv = LiveCV::Ptr(new LiveCV(parent));
    livecv->solveImportPaths();

    livecv->m_arguments->initialize(argc, argv);

    qInstallMessageHandler(&visualLogMessageHandler);

    MLNode logconfig = livecv->m_arguments->getLogConfiguration();
    for( auto it = logconfig.begin(); it != logconfig.end(); ++it ){
        if ( it.value().hasKey("toNetwork") ){
            QString toNetwork = QString::fromStdString(it.value()["toNetwork"].asString());
            int portPos = toNetwork.indexOf(':');
            if ( portPos == -1 )
                THROW_EXCEPTION(lv::Exception, "Failed to parse port in segment: " + toNetwork, -1);

            vlog().addTransport(
                QString::fromStdString(it.key()),
                new VisualLogNetworkSender(toNetwork.mid(0, portPos), toNetwork.mid(portPos + 1).toInt())
            );
            it.value().remove("toNetwork");
        }
        vlog().configure(QString::fromStdString(it.key()), it.value());
    }

    livecv->m_script = new LiveCVScript(livecv->m_arguments->scriptArguments());
    QObject::connect(
        livecv->m_project, SIGNAL(activeChanged(lv::ProjectDocument*)),
        livecv->m_script, SLOT(scriptChanged(lv::ProjectDocument*))
    );

    livecv->m_settings = Settings::create(PluginContext::configPath());
    livecv->m_settings->setLaunchMode(livecv->m_arguments->launchFlag());
    livecv->m_keymap = new KeyMap(livecv->m_settings->path());
    livecv->m_settings->addConfigFile("keymap", livecv->m_keymap);


    return livecv;
}

/**
 * Removes the application directory from the import paths. This is to avoid
 * having duplicate possible imports from within the application:
 *
 * \code
 * import live 1.0
 * \endcode
 *
 * \code
 * import plugins.live 1.0
 * \endcode
 */
void LiveCV::solveImportPaths(){

    QStringList importPaths = m_engine->engine()->importPathList();
    importPaths.removeAll(dir());
    m_engine->engine()->setImportPathList(importPaths);

    // Add the plugins directory to the import paths
    m_engine->engine()->addImportPath(PluginContext::pluginPath());
}

void LiveCV::loadQml(const QUrl &url){

    if ( m_arguments->script() != "" ){
        m_project->openProject(m_arguments->script());
    } else {
        m_project->newProject();
    }
    if ( !m_arguments->monitoredFiles().isEmpty() ){
        foreach( QString mfile, m_arguments->monitoredFiles() ){
            if ( !mfile.isEmpty() ){
                QFileInfo mfileInfo(mfile);
                if ( mfileInfo.isRelative() ){
                    m_project->openFile(
                        QDir::cleanPath(m_project->rootPath() + QDir::separator() + mfile),
                        ProjectDocument::Monitor
                    );
                } else {
                    m_project->openFile(mfile, ProjectDocument::Monitor);
                }
            }
        }
    }

    static_cast<QQmlApplicationEngine*>(m_engine->engine())->load(url);
}

void LiveCV::loadInternalPlugins(){
    qmlRegisterType<lv::ErrorHandler>(
        "base", 1, 0, "ErrorHandler");
    qmlRegisterUncreatableType<lv::LiveCV>(
        "base", 1, 0, "LiveCV", "LiveCV is available through the \'livecv\' property."
    );
    qmlRegisterUncreatableType<lv::Engine>(
        "base", 1, 0, "LiveEngine", "LiveEngine is available through \'livecv.engine\' property."
    );
    qmlRegisterUncreatableType<lv::LiveCVScript>(
        "base", 1, 0, "LiveScript", "LiveScript is available through the \'script\' property"
    );
    qmlRegisterUncreatableType<lv::Environment>(
        "base", 1, 0, "LiveEnvironment", "LiveEnvironment is available through the \'script.environment\' property."
    );
    qmlRegisterUncreatableType<lv::Settings>(
        "base", 1, 0, "LiveSettings", "LiveSettings is available through the \'livecv.settings\' property."
    );
    qmlRegisterUncreatableType<lv::Commands>(
        "base", 1, 0, "LiveCommands", "LiveCommands is available through the \'livecv.commands\' property."
    );
    qmlRegisterUncreatableType<lv::KeyMap>(
        "base", 1, 0, "KeyMap", "KeyMap is available through the \'livecv.keymap\' property."
    );
    qmlRegisterUncreatableType<lv::VisualLogModel>(
        "base", 1, 0, "VisualLogModel", "VisualLogModel is available through the \'livecv.log\' property."
    );
    qmlRegisterUncreatableType<lv::VisualLogJsObject>(
        "base", 1, 0, "VisualLog", "VisualLog is available through the \'livecv.log\' property."
    );

    m_engine->engine()->rootContext()->setContextProperty("project", m_project);
    m_engine->engine()->rootContext()->setContextProperty("script",  m_script);
    m_engine->engine()->rootContext()->setContextProperty("livecv",  this);

    QJSValue vlogjs  = m_engine->engine()->newQObject(m_vlog);
    m_engine->engine()->globalObject().setProperty("vlog", vlogjs);

    EditorPlugin ep;
    ep.registerTypes("editor");
    ep.initializeEngine(m_engine->engine(), "editor");

    m_project->addExtension(new ProjectQmlExtension(m_settings, m_project, m_engine));
}

QByteArray LiveCV::extractPluginInfo(const QString &import) const{
//    TODO: Check result
    lv::ProjectQmlExtension* qmlHandler = new lv::ProjectQmlExtension(m_settings, m_project, m_engine);

    lv::PluginInfoExtractor* extractor = qmlHandler->getPluginInfoExtractor(import);
    if ( extractor ){
        extractor->waitForResult(10000);
        if (extractor->timedOut() ){
            return "Error: Timed out\n";
        }
    }
    return extractor->result();
}

}// namespace
