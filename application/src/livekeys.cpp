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
#include "qmlscript.h"
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
#include "live/workspaceextension.h"
#include "live/packagegraph.h"

#include "live/project.h"
#include "live/documenthandler.h"
#include "live/editorprivate_plugin.h"

#include "live/projectqmlextension.h"
#include "live/codeqmlhandler.h"
#include "live/documentqmlinfo.h"
#include "live/plugininfoextractor.h"
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

namespace lv{

Livekeys::Livekeys(QObject *parent)
    : QObject(parent)
    , m_engine(new ViewEngine(new QQmlApplicationEngine))
    , m_arguments(new LivekeysArguments(header().toStdString()))
    , m_dir(QString::fromStdString(ApplicationContext::instance().applicationPath()))
    , m_project(new Project(m_engine))
    , m_settings(nullptr)
    , m_script(nullptr)
    , m_log(nullptr)
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
    connect(m_engine, &ViewEngine::applicationError, this, &Livekeys::engineError);

    VisualLog::setViewTransport(m_log);
}

Livekeys::~Livekeys(){
    VisualLog::setViewTransport(nullptr);
    delete m_settings;
    delete m_engine;
    delete m_packageGraph;
    delete m_memory;
    delete m_layers;
}

Livekeys::Ptr Livekeys::create(int argc, const char * const argv[], QObject *parent){
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

    livekeys->m_script = new QmlScript(livekeys->engine()->engine(),livekeys->m_arguments->scriptArguments());
    QObject::connect(livekeys->project(), &Project::activeChanged, livekeys->m_script, &QmlScript::scriptChanged);

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
    m_engineImportPaths = m_engine->engine()->importPathList();

    m_engineImportPaths.removeAll(dir());
    m_engineImportPaths.removeAll(QString::fromStdString(ApplicationContext::instance().executablePath()));

    // Add the plugins directory to the import paths
    m_engineImportPaths.append(QString::fromStdString(ApplicationContext::instance().pluginPath()));

    m_engine->engine()->setImportPathList(m_engineImportPaths);
}

void Livekeys::loadQml(const QUrl &url){
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

void Livekeys::loadProject(){
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
    else
        layer->setParent(m_engine);
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
    qmlRegisterUncreatableType<lv::Livekeys>(
        "base", 1, 0, "LiveKeys",        ViewEngine::typeAsPropertyMessage("LiveKeys", "lk"));
    qmlRegisterUncreatableType<lv::ViewEngine>(
        "base", 1, 0, "LiveEngine",      ViewEngine::typeAsPropertyMessage("LiveEngine", "lk.engine"));
    qmlRegisterUncreatableType<lv::QmlScript>(
        "base", 1, 0, "LiveScript",      ViewEngine::typeAsPropertyMessage("LiveScript", "script"));
    qmlRegisterUncreatableType<lv::Environment>(
        "base", 1, 0, "LiveEnvironment", ViewEngine::typeAsPropertyMessage("LiveEnvironment", "script.environment"));
    qmlRegisterUncreatableType<lv::Settings>(
        "base", 1, 0, "LiveSettings",    ViewEngine::typeAsPropertyMessage("LiveSettings", "lk.settings"));
    qmlRegisterUncreatableType<lv::VisualLogModel>(
        "base", 1, 0, "VisualLogModel",  ViewEngine::typeAsPropertyMessage("VisualLogModel", "lk.log"));
    qmlRegisterUncreatableType<lv::Memory>(
        "base", 1, 0, "Memory",          ViewEngine::typeAsPropertyMessage("Memory", "lk.mem"));
    qmlRegisterUncreatableType<lv::VisualLogQmlObject>(
        "base", 1, 0, "VisualLog",       ViewEngine::typeAsPropertyMessage("VisualLog", "vlog"));
    qmlRegisterUncreatableType<lv::VisualLogBaseModel>(
        "base", 1, 0, "VisualLogBaseModel", "VisualLogBaseModel is of abstract type.");

    ViewEngine::registerBaseTypes("base");

    m_engine->engine()->rootContext()->setContextProperty("project", m_project);
    m_engine->engine()->rootContext()->setContextProperty("script",  m_script);
    m_engine->engine()->rootContext()->setContextProperty("lk",  this);

    QJSValue vlogjs  = m_engine->engine()->newQObject(m_vlog);
    m_engine->engine()->globalObject().setProperty("vlog", vlogjs);

    ViewContext::initFromEngine(m_engine->engine());

    EditorPrivatePlugin ep;
    ep.registerTypes("editor.private");
    ep.initializeEngine(m_engine->engine(), "editor.private");
}

void Livekeys::loadInternalPackages(){
    if ( m_packageGraph )
        return;

    m_packageGraph = new PackageGraph;
    PackageGraph::internalsContextOwner() = m_packageGraph;

    m_engine->setPackageGraph(m_packageGraph);

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

void Livekeys::initializeProject(){
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
                 }/*,
                 {
                     {"package", "editlv"},
                     {"enabled", true},
                     {"component", "EditLvExtension.qml"}
                 }*/
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
              "lcvvideo"
          }},
         {"internalQtPackages",{
              "Qt",
              "QtQml",
              "QtQuick",
              "QtCanvas3D",
              "QtGraphicalEffects",
              "QtMultimedia",
              "QtWebSockets"
          }}
    };
    return config;
}

QObject *Livekeys::layerPlaceholder() const{
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

void Livekeys::engineError(QJSValue error){
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

void Livekeys::projectChanged(const QString &path){
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
