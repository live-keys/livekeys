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

#include "livekeys.h"
#include "livekeysarguments.h"
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
#include "live/workspaceextension.h"
#include "live/packagegraph.h"

#include "live/project.h"
#include "live/codehandler.h"
#include "live/editorprivate_plugin.h"

#include "qmlengineinterceptor.h"

#include "live/windowlayer.h"

#include <QUrl>
#include <QDir>
#include <QFileInfo>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QJSValue>
#include <QJSValueIterator>
#include <QQuickWindow>
#include <QGuiApplication>
#include <QProcess>

#ifdef BUILD_ELEMENTS
#include "live/elements/engine.h"
#endif

namespace lv{

Livekeys::Livekeys(QObject *parent)
    : QObject(parent)
    , m_viewEngine(new ViewEngine(new QQmlApplicationEngine))
    , m_arguments(new LivekeysArguments(header().toStdString()))
    , m_dir(QString::fromStdString(ApplicationContext::instance().applicationPath()))
#ifdef BUILD_ELEMENTS
    , m_engine(new lv::el::Engine)
#else
    , m_engine(nullptr)
#endif
    , m_project(new Project(m_engine, m_viewEngine))
    , m_settings(nullptr)
    , m_log(nullptr)
    , m_vlog(new VisualLogQmlObject) // js ownership
    , m_packageGraph(nullptr)

    , m_layers(new QQmlPropertyMap)
    , m_lastLayer(nullptr)
    , m_layerPlaceholder(nullptr)
{
    Memory::setSize(5242880);

    solveImportPaths();
    m_log = new VisualLogModel(m_viewEngine->engine());

    connect(m_project, SIGNAL(pathChanged(QString)), SLOT(projectChanged(QString)));
    connect(m_viewEngine, &ViewEngine::applicationError, this, &Livekeys::engineError);

    VisualLog::setViewTransport(m_log);
}

Livekeys::~Livekeys(){
    VisualLog::setViewTransport(nullptr);

    //TODO: Quickfix on warning when closing.
    if ( m_layers->contains("workspace") ){
        Layer* l = qobject_cast<Layer*>(m_layers->value("workspace").value<QObject*>());
        delete l->viewRoot();
    }

    Memory::clear();

    delete m_layers;
    delete m_settings;
    delete m_viewEngine;
    delete m_packageGraph;

#ifdef BUILD_ELEMENTS
    delete m_engine;
    el::Engine::dispose();
#endif
}

Livekeys::Ptr Livekeys::create(int argc, const char * const argv[], QObject *parent){
#ifdef BUILD_ELEMENTS
    std::string blobsPath = lv::ApplicationContext::instance().externalPath() + "/v8";
    lv::el::Engine::initialize(blobsPath);
#endif

    Livekeys::Ptr livekeys = Livekeys::Ptr(new Livekeys(parent));
    livekeys->m_arguments->initialize(argc, argv);

    qInstallMessageHandler(&visualLogMessageHandler);

    MLNode logconfig = livekeys->m_arguments->getLogConfiguration();
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

    ApplicationContext::instance().setScriptArguments(livekeys->m_arguments->scriptArguments());

    livekeys->m_settings = Settings::create(QString::fromStdString(ApplicationContext::instance().configPath()));

    return livekeys;
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
void Livekeys::solveImportPaths(){
    m_engineImportPaths = m_viewEngine->engine()->importPathList();

    m_engineImportPaths.removeAll(dir());
    m_engineImportPaths.removeAll(QString::fromStdString(ApplicationContext::instance().executablePath()));

    // Add the plugins directory to the import paths
    m_engineImportPaths.append(QString::fromStdString(ApplicationContext::instance().pluginPath()));

    m_viewEngine->engine()->setImportPathList(m_engineImportPaths);
}

void Livekeys::loadProject(){
    m_project->setRunSpace(layerPlaceholder());

    if ( m_arguments->script() != "" ){
        QString projPath = QString::fromStdString(m_arguments->script());
        if ( m_arguments->globalFlag() && !QFileInfo(projPath).isAbsolute() ){
            m_project->openProject(
                QString::fromStdString(ApplicationContext::instance().pluginPath()) + "/" + projPath + ".qml"
            );
        } else {
            m_project->openProject(projPath);
        }
    }
    if ( !m_arguments->monitoredFiles().isEmpty() ){
        foreach( QString mfile, m_arguments->monitoredFiles() ){
            if ( !mfile.isEmpty() ){
                QFileInfo mfileInfo(mfile);
                if ( mfileInfo.isRelative() ){
                    m_project->openTextFile(
                        QDir::cleanPath(m_project->rootPath() + QDir::separator() + mfile),
                        ProjectDocument::Monitor
                    );
                } else {
                    m_project->openTextFile(mfile, ProjectDocument::Monitor);
                }
            }
        }
    }
}

void Livekeys::addLayer(const QString &name, const QString &layer){
    m_storedLayers.insert(name, layer);
}

void Livekeys::loadLayer(const QString &name, std::function<void (Layer*)> onReady){
    auto it = m_storedLayers.find(name);
    if ( it == m_storedLayers.end() )
        THROW_EXCEPTION(Exception, "Layer not found: " + name.toStdString(), Exception::toCode("~Layer"));

    QString layerUrl = it.value();

    QFile f(layerUrl);
    if ( !f.open(QFile::ReadOnly) )
        THROW_EXCEPTION(Exception, "Failed to read file for layer: " + name.toStdString(), Exception::toCode("~File"));

    QByteArray contentBytes = f.readAll();

    QObject* layerObj = m_viewEngine->createObject(contentBytes, m_viewEngine->engine(), layerUrl);
    if ( !layerObj && m_viewEngine->lastErrors().size() > 0 )
        THROW_EXCEPTION(
            Exception, ViewEngine::toErrorString(m_viewEngine->lastErrors()).toStdString(), Exception::toCode("~Component")
        );

    if ( !layerObj )
        THROW_EXCEPTION(Exception, "Null layer returned at: " + name.toStdString(), Exception::toCode("~Layer"));

    Layer* layer = static_cast<Layer*>(layerObj);
    layer->setName(name);

    if ( !layer )
        THROW_EXCEPTION(Exception, "Object is not of layer type at: " + name.toStdString(), Exception::toCode("~Layer"));

    if ( m_lastLayer)
        layer->setParent(m_lastLayer);
    else
        layer->setParent(m_viewEngine);
    m_lastLayer = layer;
    m_layers->insert(name, QVariant::fromValue(layer));

    if ( layer->hasView() ){
        connect(layer, &Layer::viewReady, [this, onReady](Layer* l, QObject* view){
            vlog("main").v() << "Layer view ready: " << l->name();
            if ( view )
                m_layerPlaceholder = view;

            emit layerReady(l);

            if ( onReady )
                onReady(l);
        });

        layer->loadView(m_viewEngine, m_layerPlaceholder ? m_layerPlaceholder : m_viewEngine->engine());
    } else {
        vlog("main").v() << "Layer ready: " << layer->name();
        emit layerReady(layer);
        if ( onReady )
            onReady(layer);
    }
}

void Livekeys::loadLayers(const QStringList &layers, std::function<void (Layer *)> onReady){
    if ( layers.length() ){
        QStringList tail = layers;
        tail.removeFirst();
        loadLayer(layers.first(), [tail, onReady, this](Layer* l){
            if ( tail.isEmpty() ){
                if ( l->hasView() && l->nextViewParent() ){
                    m_layerPlaceholder = l->nextViewParent();
                }
                if (onReady)
                    onReady(l);
            } else {
                loadLayers(tail, onReady);
            }
        });
    }
}

int Livekeys::exec(const QGuiApplication& app){
#ifdef BUILD_ELEMENTS
    return execElements(app);
#else
    loadDefaultLayers();
    return app.exec();
#endif
}

#ifdef BUILD_ELEMENTS
int Livekeys::execElements(const QGuiApplication &app){
    int result = 0;
    m_engine->setPackageImportPaths({lv::ApplicationContext::instance().pluginPath()});
    m_engine->scope([&result, this, &app](){
        loadDefaultLayers();
        result = app.exec();
    });
    return result;
}
#else
int Livekeys::execElements(const QGuiApplication &){
    vlog().e() << "Support for elements is disabled.";
    return 0;
}
#endif

void Livekeys::loadDefaultLayers(){
    QStringList layersToLoad = {"window", "workspace", "editor"}; // defaults
    if ( !m_arguments->layers().isEmpty() ){
        layersToLoad = m_arguments->layers();
        layersToLoad.removeAll("base");
    }

    if ( !layersToLoad.isEmpty() ){
        loadLayers(layersToLoad, [this](Layer*){
            loadProject();
        });
    } else {
        loadProject();
    }
}

void Livekeys::loadInternals(){
    loadInternalPackages();
    loadInternalPlugins();
    addDefaultLayers();
    QmlEngineInterceptor::interceptEngine(engine(), m_packageGraph, m_project);
}

void Livekeys::loadInternalPlugins(){
//    qmlRegisterUncreatableType<lv::Livekeys>(
//        "base", 1, 0, "LiveKeys",        ViewEngine::typeAsPropertyMessage("LiveKeys", "lk"));

    m_viewEngine->engine()->rootContext()->setContextProperty("project", m_project);
    m_viewEngine->engine()->rootContext()->setContextProperty("lk",  this);

    QJSValue vlogjs  = m_viewEngine->engine()->newQObject(m_vlog);
    m_viewEngine->engine()->globalObject().setProperty("vlog", vlogjs);

    EditorPrivatePlugin ep;
    ep.registerTypes("editor.private");
    ep.initializeEngine(m_viewEngine, m_settings, "editor.private");
}

void Livekeys::loadInternalPackages(){
    if ( m_packageGraph )
        return;

    m_packageGraph = new PackageGraph;
    PackageGraph::internalsContextOwner() = m_packageGraph;
    m_packageGraph->setPackageImportPaths(packageImportPaths());

    m_viewEngine->setPackageGraph(m_packageGraph);

    const MLNode& defaults = ApplicationContext::instance().startupConfiguration();
    if ( defaults.hasKey("internalPackages") ){
        const MLNode& internalPackages = defaults["internalPackages"];

        for ( auto it = internalPackages.asArray().begin(); it != internalPackages.asArray().end(); ++it ){

            std::string packagePath = ApplicationContext::instance().pluginPath() + "/" + it->asString();

            if ( Package::existsIn(packagePath) ){
                PackageGraph::addInternalPackage(Package::createFromPath(packagePath));
            }
        }
    }

    if ( defaults.hasKey("internalQtPackages") ){
        const MLNode& qtPackages = defaults["internalQtPackages"];
        for ( auto it = qtPackages.asArray().begin(); it != qtPackages.asArray().end(); ++it ){
            Package::Ptr package = Package::createFromNode(it->asString(), "", {
                {"name", *it},
                {"version", QT_VERSION_STR},
                {"documentation", "editor/loadqtdocs.qml"}
            });
            PackageGraph::addInternalPackage(package);
        }
    }
}

void Livekeys::addDefaultLayers(){
    const MLNode& defaults = ApplicationContext::instance().startupConfiguration();
    if ( defaults.hasKey("layers") ){
        const MLNode& layers = defaults["layers"];
        for ( auto it = layers.begin(); it != layers.end(); ++it ){
            addLayer(QString::fromStdString(it.key()), QString::fromStdString(it.value().asString()));
        }
    }
}

std::vector<std::string> Livekeys::packageImportPaths() const{
    std::vector<std::string> paths;
    paths.push_back(ApplicationContext::instance().pluginPath());

    if ( !m_project->rootPath().isEmpty() ){
        paths.push_back(m_project->dir().toStdString() + "/packages");
    }
    return paths;
}

QQmlPropertyMap *Livekeys::layers(){
    return m_layers;
}

const MLNode &Livekeys::startupConfiguration(){
    static MLNode config = {
        {"layers", {
             {"window",    ":/windowlayer.qml"},
             {"workspace", ":/workspacelayer.qml"},
             {"editor",    ":/editorlayer.qml"}
         }},
        {"workspace", {
            {"extensions", {
                 {
                     {"package", "editqml"},
                     {"enabled", true},
                     {"component", "EditQmlExtension.qml"}
                 },
                 {
                     {"package", "workspace"},
                     {"enabled", true},
                     {"component", "WorkspaceControlsExtension.qml"}
                 },
                 {
                     {"package", "lcvcore"},
                     {"enabled", true},
                     {"component", "EditCvExtension.qml"}
                 }/*,
                 {
                     {"package", "editlv"},
                     {"enabled", true},
                     {"component", "EditLvExtension.qml"}
                 },
                 {
                     {"package", "editjson"},
                     {"enabled", true},
                     {"component", "EditJsonExtension.qml"}
                 }*/
            }},
            {"tutorials", {
                 {
                     {"package", "squareone"},
                     {"enabled", true}
                 }
            }},
            {"samples", {
                 {{"package", "lcvcore"}, {"enabled", true}},
                 {{"package", "fs"}, {"enabled", true}},
                 {{"package", "base"}, {"enabled", true}},
                 {{"package", "live"}, {"enabled", false}},
                 {{"package", "lcvimgproc"},{"enabled", false}},
                 {{"package", "lcvvideo"},{"enabled", false}},
                 {{"package", "lcvphoto"},{"enabled", false}},
                 {{"package", "lcvfeatures2d"}, {"enabled", false}},
                 {{"package", "workspace"}, {"enabled", false}},
                 {{"package", "editor"}, {"enabled", false}}
            }}
         }},
        {"timeline",{
             {"loaders", {
                  {"VideoCapture", "lcvcore/VideoCaptureSegmentCreator.qml"}
             }}
        }},
         {"internalPackages",{
              "editor",
              "editqml",
              "fs",
              "live",
              "lcvcore",
              "lcvfeatures2d",
              "lcvimgproc",
              "lcvphoto",
              "lcvvideo",
              "timeline",
              "workspace"
          }},
         {"internalQtPackages",{
              "Qt",
              "QtQml",
              "QtQuick",
              "QtCanvas3D",
              "QtGraphicalEffects",
              "QtMultimedia",
              "QtWebSockets",
              "QtWebEngine"
          }}
    };
    return config;
}

QObject *Livekeys::layerPlaceholder() const{
    if ( !m_layerPlaceholder ){
        QList<QObject*> rootObjects = static_cast<QQmlApplicationEngine*>(m_viewEngine->engine())->rootObjects();
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

void Livekeys::engineError(QJSValue error){
    QmlError e(m_viewEngine, error);

    vlog().e() <<  "Uncaught error: " + e.toString(
        QmlError::PrintMessage | QmlError::PrintLocation | QmlError::PrintStackTrace
    );
    vlog().d() << e.toString(QmlError::PrintCStackTrace);
}

void Livekeys::projectChanged(const QString &path){
    m_viewEngine->engine()->setImportPathList(m_engineImportPaths);
    if ( !path.isEmpty() && QFileInfo(path + "/packages").isDir() )
        m_viewEngine->engine()->addImportPath(path + "/packages");

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

void Livekeys::newProjectInstance(){
    QProcess* fork = new QProcess();

    QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
    fork->setProcessEnvironment(env);

    QString program = "./livekeys";

    QStringList arguments;

    std::vector<std::string> args = m_arguments->arguments();
    for ( const std::string& arg : args ){
        arguments.append(QString::fromStdString(arg));
    }

    fork->startDetached(program, arguments);
    fork->waitForStarted();
    QCoreApplication::exit(0);
}

void Livekeys::openProjectInstance(const QUrl &path){
    QProcess* fork = new QProcess();

    QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
    fork->setProcessEnvironment(env);

    QString program = "./livekeys";

    QStringList arguments;

    std::vector<std::string> args = m_arguments->arguments();
    for ( const std::string& arg : args ){
        arguments.append(QString::fromStdString(arg));
    }
    arguments.append(path.toLocalFile());

    fork->startDetached(program, arguments);
    fork->waitForStarted();
    QCoreApplication::exit(0);
}

}// namespace
