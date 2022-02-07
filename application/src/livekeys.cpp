/****************************************************************************
**
** Copyright (C) 2022 Dinu SV.
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

#include "live/visuallog.h"
#include "live/visuallogqt.h"
#include "live/visuallogmodel.h"
#include "live/visuallognetworksender.h"
#include "live/viewengine.h"
#include "live/settings.h"
#include "live/applicationcontext.h"
#include "live/viewcontext.h"
#include "live/windowlayer.h"
#include "live/packagegraph.h"

#include "live/program.h"
#include "live/qmlprogram.h"
#include "live/libraryloadpath.h"

#include <QUrl>
#include <QDir>
#include <QQmlApplicationEngine>
#include <QFileInfo>
#include <QQmlContext>
#include <QJSValue>
#include <QJSValueIterator>
#include <QQuickWindow>
#include <QGuiApplication>
#include <QProcess>

#ifdef BUILD_ELEMENTS
#include "live/elements/compiler/compiler.h"

#ifdef BUILD_ELEMENTS_ENGINE
#include "live/elements/engine.h"
#endif

#endif

namespace lv{

Livekeys::Livekeys(QObject *parent)
    : QObject(parent)
    , m_lockedFileIO(LockedFileIOSession::createInstance())
    , m_viewEngine(new ViewEngine(new QQmlApplicationEngine, m_lockedFileIO))
    , m_arguments(new LivekeysArguments(header().toStdString()))
    , m_dir(QString::fromStdString(ApplicationContext::instance().applicationPath()))
#ifdef BUILD_ELEMENTS_ENGINE
    , m_engine(new lv::el::Engine)
#else
    , m_engine(nullptr)
#endif
    , m_main(nullptr)
    , m_settings(nullptr)
    , m_log(nullptr)
    , m_packageGraph(nullptr)

    , m_layers(new QQmlPropertyMap)
    , m_lastLayer(nullptr)
    , m_layerPlaceholder(nullptr)
{
    Memory::setSize(5242880);
    solveImportPaths();
    connect(m_viewEngine, &ViewEngine::applicationError, this, &Livekeys::engineError);

    m_log = new VisualLogModel(m_viewEngine->engine());

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

#ifdef BUILD_ELEMENTS_ENGINE
    delete m_engine;
    el::Engine::dispose();
#endif

    delete m_main;
}

Livekeys::Ptr Livekeys::create(int argc, const char * const argv[], QObject *parent){
#ifdef BUILD_ELEMENTS_ENGINE
    std::string blobsPath = lv::ApplicationContext::instance().externalPath() + "/v8";
    lv::el::Engine::initialize(blobsPath);
#endif

    Livekeys::Ptr livekeys = Livekeys::Ptr(new Livekeys(parent));
    livekeys->m_arguments->initialize(argc, argv);

    QString mainPath = livekeys->scriptPath();
    if ( !mainPath.isEmpty() ){
        QmlProgram* qmlMain = QmlProgram::create(livekeys->m_viewEngine, mainPath.toStdString());
        auto scriptArguments = livekeys->m_arguments->scriptArguments();
        QStringList arguments;
        for ( const std::string& arg: scriptArguments ){
            arguments.push_back(QString::fromStdString(arg));
        }
        qmlMain->setArguments(arguments);
        livekeys->m_main = qmlMain;
    }

    qInstallMessageHandler(&visualLogMessageHandler);

    MLNode logconfig = livekeys->m_arguments->logConfiguration();
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
    QmlProgram* qmlMain = static_cast<QmlProgram*>(m_main);
    setMain(qmlMain);
    if ( qmlMain ){
        qmlMain->setRunSpace(layerPlaceholder());
        qmlMain->run();
    }

//    m_project->monitorFiles(m_arguments->monitoredFiles());
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
    if ( !layer )
        THROW_EXCEPTION(Exception, "Object is not of layer type at: " + name.toStdString(), Exception::toCode("~Layer"));

    layer->setName(name);
    layer->initialize(m_arguments->layerConfigurationFor(name.toStdString()));

    if ( m_lastLayer)
        layer->setParent(m_lastLayer);
    else
        layer->setParent(m_viewEngine);
    m_lastLayer = layer;
    m_layers->insert(name, QVariant::fromValue(layer));

    if ( layer->hasView() ){
        connect(layer, &Layer::viewReady, [this, onReady](Layer* l, QObject* view){
            vlog("main").v() << "Layer view ready: " << l->name();

            if ( view ){
                QObject* nextViewParent = l->nextViewParent();
                if ( nextViewParent )
                    m_layerPlaceholder = nextViewParent;
            }

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
    loadConfiguredLayers();
    return app.exec();
#endif
}

void Livekeys::setMain(Program *program){
    if ( program == m_main )
        return;

    m_main = program;
    m_viewEngine->engine()->setImportPathList(m_engineImportPaths);
    m_packageGraph->clearPackages();

    if ( m_main ){
        QString path = QString::fromStdString(m_main->rootPath().data());
        if ( !path.isEmpty() && QFileInfo(path + "/packages").isDir() )
            m_viewEngine->engine()->addImportPath(path + "/packages");

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
}

Program *Livekeys::main(){
    return m_main;
}

QString Livekeys::scriptPath() const{
    if ( m_arguments->script() != "" ){
        QString projPath = QString::fromStdString(m_arguments->script());
        if ( m_arguments->globalFlag() && !QFileInfo(projPath).isAbsolute() ){
            return QString::fromStdString(ApplicationContext::instance().pluginPath()) + "/" + projPath + ".qml";
        } else {
            return projPath;
        }
    }
    return QString();
}

#ifdef BUILD_ELEMENTS_ENGINE
int Livekeys::execElements(const QGuiApplication &app){
    int result = 0;
    m_engine->compiler()->setPackageImportPaths({lv::ApplicationContext::instance().pluginPath()});
    m_engine->scope([&result, this, &app](){
        loadConfiguredLayers();
        result = app.exec();
    });
    return result;
}
#else
int Livekeys::execElements(const QGuiApplication &){
    vlog("main").e() << "Support for elements is disabled.";
    return 0;
}
#endif

void Livekeys::loadConfiguredLayers(){
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
}

void Livekeys::loadInternalPlugins(){
//    qmlRegisterUncreatableType<lv::Livekeys>(
//        "base", 1, 0, "LiveKeys",        ViewEngine::typeAsPropertyMessage("LiveKeys", "lk"));

    m_viewEngine->engine()->rootContext()->setContextProperty("lk",  this);
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

int Livekeys::run(const QGuiApplication &app){
    if ( m_arguments->isCommand() ){
        if ( m_arguments->command() == LivekeysArguments::Help ){
            printf("%s", m_arguments->helpString().c_str());
            return 0;
        } else if ( m_arguments->command() == LivekeysArguments::Version ){
            printf("%s\n", qPrintable(versionString()));
            return 0;
        } else if ( m_arguments->command() == LivekeysArguments::Compile ){
#ifdef BUILD_ELEMENTS
            lv::el::Compiler::Config config;
            config.initialize(m_arguments->compileConfiguration());

            lv::el::Compiler::Ptr compiler = lv::el::Compiler::create(config);

            try{
                std::string filePath = scriptPath().toStdString();

                QFileInfo finfo(QString::fromStdString(filePath));
                std::string pluginPath = finfo.path().toStdString();

                Module::Ptr module(nullptr);
                if ( Module::existsIn(pluginPath) ){
                    module = Module::createFromPath(pluginPath);
                    Package::Ptr package = Package::createFromPath(module->package());
                    if ( package ){
                        compiler->setPackageImportPaths({package->path() + "/" + compiler->importLocalPath()});
                    }
                }
                lv::el::Compiler::compile(compiler, filePath);
            } catch ( lv::el::SyntaxException& e ){
                vlog().e() << "Compiler Error: " << e.message() << " (" << e.parsedFile() << ":" << e.parsedLine() << ")";
            } catch ( lv::Exception& e ){
                vlog().e() << "Compiler Error: " << e.message();
            }
            return 0;
#endif
        }

        return -1;
    }

    LibraryLoadPath::addRecursive(ApplicationContext::instance().pluginPath(), ApplicationContext::instance().linkPath());
    if ( QFileInfo::exists(QString::fromStdString(ApplicationContext::instance().externalPath())) )
        LibraryLoadPath::addRecursive(ApplicationContext::instance().externalPath(), ApplicationContext::instance().linkPath());

    return exec(app);
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

    if ( m_main && !m_main->rootPath().isEmpty() ){
        paths.push_back((m_main->rootPath() + "/packages").data());
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
    return m_layerPlaceholder;
}

void Livekeys::engineError(QJSValue error){
    QmlError e(m_viewEngine, error);

    vlog("main").e() <<  "Uncaught error: " + e.toString(
        QmlError::PrintMessage | QmlError::PrintLocation | QmlError::PrintStackTrace
    );
    vlog("main").d() << e.toString(QmlError::PrintCStackTrace);
}

}// namespace
