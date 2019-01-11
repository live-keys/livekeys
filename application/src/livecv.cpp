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

#include "livecv.h"
#include "livecvarguments.h"
#include "livecvscript.h"
#include "commands.h"
#include "environment.h"
#include "live/extensions.h"

#include "live/keymap.h"
#include "live/visuallog.h"
#include "live/visuallogqt.h"
#include "live/visuallogmodel.h"
#include "live/visuallognetworksender.h"
#include "live/visuallogqmlobject.h"
#include "live/viewengine.h"
#include "live/errorhandler.h"
#include "live/settings.h"
#include "live/applicationcontext.h"
#include "live/viewcontext.h"
#include "live/liveextension.h"
#include "live/packagegraph.h"

#include "live/editorprivate_plugin.h"
#include "live/project.h"
#include "live/documenthandler.h"

#include "live/projectqmlextension.h"
#include "live/codeqmlhandler.h"
#include "live/documentqmlinfo.h"
#include "live/plugininfoextractor.h"
#include "live/qmlengineinterceptor.h"

#include <QUrl>
#include <QFileInfo>
#include <QDir>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QJSValue>
#include <QJSValueIterator>
#include <QQuickWindow>
#include <QGuiApplication>


namespace lv{

LiveCV::LiveCV(QObject *parent)
    : QObject(parent)
    , m_engine(new ViewEngine(new QQmlApplicationEngine))
    , m_arguments(new LiveCVArguments(header().toStdString()))
    , m_codeInterface(0)
    , m_dir(QString::fromStdString(ApplicationContext::instance().applicationPath()))
    , m_project(new Project)
    , m_settings(nullptr)
    , m_script(nullptr)
    , m_commands(new Commands)
    , m_keymap(nullptr)
    , m_extensions(nullptr)
    , m_log(0)
    , m_vlog(new VisualLogQmlObject) // js ownership
    , m_packageGraph(nullptr)
    , m_windowControls(nullptr)
{
    solveImportPaths();
    m_log = new VisualLogModel(m_engine->engine());

    connect(m_project, SIGNAL(pathChanged(QString)), SLOT(projectChanged(QString)));
    connect(m_engine, &ViewEngine::applicationError, this, &LiveCV::engineError);

    VisualLog::setViewTransport(m_log);
}

LiveCV::~LiveCV(){
    delete m_settings;
    delete m_engine;
    delete m_packageGraph;
}

LiveCV::Ptr LiveCV::create(int argc, const char * const argv[], QObject *parent){
    LiveCV::Ptr livecv = LiveCV::Ptr(new LiveCV(parent));

    livecv->m_arguments->initialize(argc, argv);

    qInstallMessageHandler(&visualLogMessageHandler);

    MLNode logconfig = livecv->m_arguments->getLogConfiguration();
    for( auto it = logconfig.begin(); it != logconfig.end(); ++it ){
        if ( it.value().hasKey("toNetwork") ){
            QString toNetwork = QString::fromStdString(it.value()["toNetwork"].asString());
            int portPos = toNetwork.indexOf(':');
            if ( portPos == -1 )
                THROW_EXCEPTION(lv::Exception, "Failed to parse port in segment: " + toNetwork.toStdString(), -1);

            vlog().addTransport(
                it.key(),
                new VisualLogNetworkSender(toNetwork.mid(0, portPos), toNetwork.mid(portPos + 1).toInt())
            );

            if ( !it.value().hasKey("toExtensions") )
                it.value()["toExtensions"] = true;
            it.value().remove("toNetwork");
        }
        vlog().configure(it.key(), it.value());
    }

    livecv->m_script = new LiveCVScript(livecv->m_arguments->scriptArguments());
    QObject::connect(
        livecv->m_project, SIGNAL(activeChanged(lv::ProjectDocument*)),
        livecv->m_script, SLOT(scriptChanged(lv::ProjectDocument*))
    );

    livecv->m_settings = Settings::create(QString::fromStdString(ApplicationContext::instance().configPath()));
    livecv->m_settings->setLaunchMode(livecv->m_arguments->launchFlag());
    livecv->m_keymap = new KeyMap(livecv->m_settings->path());
    livecv->m_settings->addConfigFile("keymap", livecv->m_keymap);

    livecv->m_extensions = new Extensions(livecv->m_engine, livecv->m_settings->path());
    livecv->m_settings->addConfigFile("extensions", livecv->m_extensions);

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
    importPaths.removeAll(QString::fromStdString(ApplicationContext::instance().executablePath()));

    m_engine->engine()->setImportPathList(importPaths);

    // Add the plugins directory to the import paths
    m_engine->engine()->addImportPath(QString::fromStdString(ApplicationContext::instance().pluginPath()));
}

void LiveCV::loadQml(const QUrl &url){

    if ( m_arguments->script() != "" ){
        m_project->openProject(QString::fromStdString(m_arguments->script()));
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

void LiveCV::loadInternals(){
    loadInternalPackages();
    loadInternalPlugins();
    QmlEngineInterceptor::interceptEngine(engine(), m_packageGraph);
}

void LiveCV::loadInternalPlugins(){
    qmlRegisterType<lv::ErrorHandler>(
        "base", 1, 0, "ErrorHandler");
    qmlRegisterUncreatableType<lv::LiveCV>(
        "base", 1, 0, "LiveCV", "LiveCV is available through the \'livecv\' property."
    );
    qmlRegisterUncreatableType<lv::ViewEngine>(
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
    qmlRegisterUncreatableType<lv::VisualLogBaseModel>(
        "base", 1, 0, "VisualLogBaseModel", "VisualLogBaseModel is of abstract type."
    );
    qmlRegisterUncreatableType<lv::VisualLogModel>(
        "base", 1, 0, "VisualLogModel", "VisualLogModel is available through the \'livecv.log\' property."
    );
    qmlRegisterUncreatableType<lv::VisualLogQmlObject>(
        "base", 1, 0, "VisualLog", "VisualLog is available through the \'livecv.log\' property."
    );

    m_engine->engine()->rootContext()->setContextProperty("project", m_project);
    m_engine->engine()->rootContext()->setContextProperty("script",  m_script);
    m_engine->engine()->rootContext()->setContextProperty("livecv",  this);

    QJSValue vlogjs  = m_engine->engine()->newQObject(m_vlog);
    m_engine->engine()->globalObject().setProperty("vlog", vlogjs);

//    ApplicationContext::initialize(m_engine, m_settings); TODO: Come back
    ViewContext::initFromEngine(m_engine->engine());

    EditorPrivatePlugin ep;
    ep.registerTypes("editor.private");
    ep.initializeEngine(m_engine->engine(), "editor.private");

    m_extensions->loadExtensions();
    for ( auto it = m_extensions->begin(); it != m_extensions->end(); ++it ){
        LiveExtension* le = it.value();
        m_commands->add(le, le->commands());
        m_keymap->store(le->keyBindings());
    }

}

void LiveCV::loadInternalPackages(){
    if ( m_packageGraph )
        return;

    m_packageGraph = new PackageGraph;
    PackageGraph::internalsContextOwner() = m_packageGraph;

    std::vector<std::string> internalPackages = {
        "editor",
        "editqml",
        "live",
        "lcvcore",
        "lcvfeatures2d",
        "lcvimgproc",
        "lcvphoto",
        "lcvvideo"
    };

    for ( auto it = internalPackages.begin(); it != internalPackages.end(); ++it ){

        std::string packagePath = ApplicationContext::instance().pluginPath() + "/" + *it;

        if ( Package::existsIn(packagePath) ){
            PackageGraph::addInternalPackage(Package::createFromPath(packagePath));
        }
    }

    std::vector<std::string> qtPackages = {
        "Qt",
        "QtQml",
        "QtQuick",
        "QtCanvas3D",
        "QtGraphicalEffects",
        "QtMultimedia",
        "QtWebSockets"
    };

    for ( auto it = qtPackages.begin(); it != qtPackages.end(); ++it ){
        Package::Ptr package = Package::createFromNode(*it, "", {
            {"name", *it},
            {"version", QT_VERSION_STR}
        });
        PackageGraph::addInternalPackage(package);
    }
}

std::vector<std::string> LiveCV::packageImportPaths() const{
    std::vector<std::string> paths;
    paths.push_back(ApplicationContext::instance().pluginPath());

    if ( !m_project->rootPath().isEmpty() ){
        paths.push_back(m_project->dir().toStdString() + "/packages");
    }
    return paths;
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

QQmlPropertyMap *LiveCV::extensions(){
    if ( m_extensions )
        return m_extensions->globals();
    return nullptr;
}

QObject *LiveCV::windowControls() const{
    if ( !m_windowControls ){
        QList<QObject*> rootObjects = static_cast<QQmlApplicationEngine*>(m_engine->engine())->rootObjects();
        for ( auto it = rootObjects.begin(); it != rootObjects.end(); ++it ){
            if ( (*it)->objectName() == "window" ){
                m_windowControls = (*it)->property("controls").value<QObject*>();
            }
        }
    }
    return m_windowControls;
}

QJSValue LiveCV::interceptMenu(QJSValue context){

    QJSValueList interceptorArgs;
    interceptorArgs << context;

    QJSValueList result;

    for ( auto it = m_extensions->begin(); it != m_extensions->end(); ++it ){
        LiveExtension* le = it.value();
        if ( le->hasMenuInterceptor() ){
            QJSValue v = le->callMenuInterceptor(interceptorArgs);
            if ( v.isArray() ){
                QJSValueIterator it(v);
                while ( it.hasNext() ){
                    it.next();
                    if ( it.name() != "length" ){
                        result << it.value();
                    }
                }
            }
        }
    }

    QJSValue concat = m_engine->engine()->newArray(result.length());
    int index = 0;
    for ( auto it = result.begin(); it != result.end(); ++it ){
        concat.setProperty(index, *it);
        ++index;
    }

    return concat;
}

void LiveCV::engineError(QJSValue error){
    QString message = "Uncaught error: " +
            error.property("message").toString() +
            "(code:" + error.property("code").toString() + ")";

    if ( error.hasOwnProperty("fileName") ){
        message += "\nat " +
                error.property("fileName").toString() + ":" +
                error.property("lineNumber").toString() + "@" +
                error.property("functionName").toString();
    }

    if ( error.hasOwnProperty("stackTrace") ){
        message += "\nStackTrace:";
        QJSValueIterator stackIt(error.property("stackTrace"));
        while ( stackIt.hasNext() ){
            stackIt.next();
            message += "\n" + stackIt.value().toString();
        }
    } else if ( error.hasOwnProperty("stack") ){
        message += "\nStackTrace:\n" + error.property("stack").toString();
    }

    vlog().e() << message;
}

void LiveCV::projectChanged(const QString &path){
    m_packageGraph->clearPackages();
    m_packageGraph->setPackageImportPaths(packageImportPaths());
    std::string packagePath = path.toStdString();
    if ( Package::existsIn(packagePath) ){
        std::list<Package::Reference> missing;
        m_packageGraph->loadPackageWithDependencies(Package::createFromPath(packagePath), missing);
        if ( missing.size() > 0 ){
            emit missingPackages();
        }
    }
}

}// namespace
