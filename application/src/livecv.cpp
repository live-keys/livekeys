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
#include "environment.h"
#include "live/memory.h"

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
#include "qmlengineinterceptor.h"

#include "live/windowlayer.h"

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
    , m_dir(QString::fromStdString(ApplicationContext::instance().applicationPath()))
    , m_project(new Project(m_engine))
    , m_settings(nullptr)
    , m_script(nullptr)
    , m_log(0)
    , m_vlog(new VisualLogQmlObject) // js ownership
    , m_packageGraph(nullptr)
    , m_memory(new Memory(this))
    , m_layers(new QQmlPropertyMap)
    , m_lastLayer(nullptr)
    , m_layerPlaceholder(nullptr)
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
    delete m_memory;
    delete m_layers;
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
                THROW_EXCEPTION(lv::Exception, "Failed to parse port in segment: " + toNetwork.toStdString(), Exception::toCode("Init"));

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
    m_engineImportPaths = m_engine->engine()->importPathList();

    m_engineImportPaths.removeAll(dir());
    m_engineImportPaths.removeAll(QString::fromStdString(ApplicationContext::instance().executablePath()));

    // Add the plugins directory to the import paths
    m_engineImportPaths.append(QString::fromStdString(ApplicationContext::instance().pluginPath()));

    m_engine->engine()->setImportPathList(m_engineImportPaths);
}

void LiveCV::loadQml(const QUrl &url){
    static_cast<QQmlApplicationEngine*>(m_engine->engine())->load(url);

    m_project->setRunSpace(layerPlaceholder());

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
}

void LiveCV::loadProject(){
    m_project->setRunSpace(layerPlaceholder());

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
}

void LiveCV::addLayer(const QString &name, const QString &layer){
    m_storedLayers.insert(name, layer);
}

void LiveCV::loadLayer(const QString &name, std::function<void (Layer*)> onReady){
    auto it = m_storedLayers.find(name);
    if ( it == m_storedLayers.end() )
        THROW_EXCEPTION(Exception, "Layer not found: " + name.toStdString(), Exception::toCode("~Layer"));

    QString layerUrl = it.value();

    QFile f(layerUrl);
    if ( !f.open(QFile::ReadOnly) )
        THROW_EXCEPTION(Exception, "Failed to read file for layer: " + name.toStdString(), Exception::toCode("~File"));

    QByteArray contentBytes = f.readAll();

    QObject* layerObj = m_engine->createObject(contentBytes, m_engine->engine(), layerUrl);
    if ( !layerObj && m_engine->lastErrors().size() > 0 )
        THROW_EXCEPTION(
            Exception, ViewEngine::toErrorString(m_engine->lastErrors()).toStdString(), Exception::toCode("~Component")
        );

    if ( !layerObj )
        THROW_EXCEPTION(Exception, "Null layer returned at: " + name.toStdString(), Exception::toCode("~Layer"));

    Layer* layer = static_cast<Layer*>(layerObj);
    layer->setName(name);

    if ( !layer )
        THROW_EXCEPTION(Exception, "Object is not of layer type at: " + name.toStdString(), Exception::toCode("~Layer"));

    if ( m_lastLayer)
        layer->setParent(m_lastLayer);
    m_lastLayer = layer;
    m_layers->insert(name, QVariant::fromValue(layer));

    if ( layer->hasView() ){
        connect(layer, &Layer::viewReady, [this, onReady](Layer* layer, QObject* view){
            vlog("main").v() << "Layer view ready: " << layer->name();
            if ( view )
                m_layerPlaceholder = view;

            emit layerReady(layer);

            if ( onReady )
                onReady(layer);
        });

        layer->loadView(m_engine, m_layerPlaceholder ? m_layerPlaceholder : m_engine->engine());
    } else {
        vlog("main").v() << "Layer ready: " << layer->name();
        emit layerReady(layer);
        if ( onReady )
            onReady(layer);
    }
}

void LiveCV::loadLayers(const QStringList &layers, std::function<void (Layer *)> onReady){
    if ( layers.length() ){
        QStringList tail = layers;
        tail.removeFirst();
        loadLayer(layers.first(), [tail, onReady, this](Layer* l){
            if ( tail.isEmpty() ){
                if ( l->hasView() && l->nextViewParent() ){
                    m_layerPlaceholder = l->nextViewParent();
                    qDebug() << "Assigned layer placeholder";
                }
                if (onReady)
                    onReady(l);
            } else {
                loadLayers(tail, onReady);
            }
        });
    }
}

void LiveCV::loadInternals(){
    loadInternalPackages();
    loadInternalPlugins();
    addDefaultLayers();
    QmlEngineInterceptor::interceptEngine(engine(), m_packageGraph, m_project);
}

void LiveCV::loadInternalPlugins(){
    qmlRegisterType<lv::ErrorHandler>("base", 1, 0, "ErrorHandler");
    qmlRegisterUncreatableType<lv::LiveCV>(
         "base", 1, 0, "LiveCV",         ViewEngine::typeAsPropertyMessage("LiveCV", "livecv"));
    qmlRegisterUncreatableType<lv::ViewEngine>(
        "base", 1, 0, "LiveEngine",      ViewEngine::typeAsPropertyMessage("LiveEngine", "livecv.engine"));
    qmlRegisterUncreatableType<lv::LiveCVScript>(
        "base", 1, 0, "LiveScript",      ViewEngine::typeAsPropertyMessage("LiveScript", "script"));
    qmlRegisterUncreatableType<lv::Environment>(
        "base", 1, 0, "LiveEnvironment", ViewEngine::typeAsPropertyMessage("LiveEnvironment", "script.environment"));
    qmlRegisterUncreatableType<lv::Settings>(
        "base", 1, 0, "LiveSettings",    ViewEngine::typeAsPropertyMessage("LiveSettings", "livecv.settings"));
    qmlRegisterUncreatableType<lv::VisualLogModel>(
        "base", 1, 0, "VisualLogModel",  ViewEngine::typeAsPropertyMessage("VisualLogModel", "livecv.log"));
    qmlRegisterUncreatableType<lv::Memory>(
        "base", 1, 0, "Memory", ViewEngine::typeAsPropertyMessage("Memory", "livecv.mem"));
    qmlRegisterUncreatableType<lv::VisualLogQmlObject>(
        "base", 1, 0, "VisualLog",       ViewEngine::typeAsPropertyMessage("VisualLog", "vlog"));
    qmlRegisterUncreatableType<lv::VisualLogBaseModel>(
        "base", 1, 0, "VisualLogBaseModel", "VisualLogBaseModel is of abstract type.");

    ViewEngine::registerBaseTypes("base");

    m_engine->engine()->rootContext()->setContextProperty("project", m_project);
    m_engine->engine()->rootContext()->setContextProperty("script",  m_script);
    m_engine->engine()->rootContext()->setContextProperty("livecv",  this);

    QJSValue vlogjs  = m_engine->engine()->newQObject(m_vlog);
    m_engine->engine()->globalObject().setProperty("vlog", vlogjs);

    ViewContext::initFromEngine(m_engine->engine());

    EditorPrivatePlugin ep;
    ep.registerTypes("editor.private");
    ep.initializeEngine(m_engine->engine(), "editor.private");
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

void LiveCV::initializeProject(){
    m_project->setRunSpace(layerPlaceholder());

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
}

void LiveCV::addDefaultLayers(){
    addLayer("window", ":/windowlayer.qml");
    addLayer("workspace", ":/workspacelayer.qml");
    addLayer("editor", ":/editorlayer.qml");
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
    lv::ProjectQmlExtension* qmlHandler = new lv::ProjectQmlExtension();
    qmlHandler->setParams(m_settings, m_project, m_engine);

    lv::PluginInfoExtractor* extractor = qmlHandler->getPluginInfoExtractor(import);
    if ( extractor ){
        extractor->waitForResult(10000);
        if (extractor->timedOut() ){
            return "Error: Timed out\n";
        }
    }

    return extractor->result();
}

QQmlPropertyMap *LiveCV::layers(){
    return m_layers;
}

QObject *LiveCV::layerPlaceholder() const{
    if ( !m_layerPlaceholder ){
        QList<QObject*> rootObjects = static_cast<QQmlApplicationEngine*>(m_engine->engine())->rootObjects();
        for ( auto it = rootObjects.begin(); it != rootObjects.end(); ++it ){
            if ( (*it)->objectName() == "window" ){
                QObject* controls = (*it)->property("controls").value<QObject*>();
                QObject* workspace = controls->property("workspace").value<QObject*>();
                QObject* prj = workspace->property("project").value<QObject*>();
                m_layerPlaceholder = prj->property("runSpace").value<QObject*>();
            }
        }
    }
    return m_layerPlaceholder;
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
    m_engine->engine()->setImportPathList(m_engineImportPaths);
    if ( !path.isEmpty() && QFileInfo(path + "/packages").isDir() )
        m_engine->engine()->addImportPath(path + "/packages");

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
