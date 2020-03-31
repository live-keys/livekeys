#include "workspacelayer.h"
#include "live/visuallogqt.h"
#include "live/viewengine.h"
#include "live/exception.h"
#include "live/viewcontext.h"
#include "live/settings.h"
#include "live/projectfile.h"
#include "live/workspaceextension.h"
#include "live/project.h"
#include "live/projectfile.h"
#include "live/theme.h"
#include "live/windowlayer.h"
#include "live/applicationcontext.h"

#include "live/mlnode.h"
#include "live/mlnodetoqml.h"
#include "live/mlnodetojson.h"

#include "workspace.h"
#include "projectworkspace.h"
#include "documentation.h"

#include <QQuickItem>
#include <QQmlEngine>
#include <QQmlContext>
#include <QQmlPropertyMap>
#include <QJSValueIterator>
#include <QJSValue>
#include <QFileInfo>
#include <QFile>
#include <QUrl>

namespace lv{

//HERE

// TODO: load samples
// TODO: add live.package.json file to workspace
// TODO: Remove project::newProject being triggered
// TODO: Create new default qml template for startup screen.

WorkspaceLayer::WorkspaceLayer(QObject *parent)
    : Layer(parent)
    , m_projectEnvironment(nullptr)
    , m_panes(nullptr)
    , m_viewRoot(nullptr)
    , m_commands(new Commands)
    , m_keymap(nullptr)
    , m_themes(new ThemeContainer("workspace", this))
    , m_project(nullptr)
    , m_extensions(nullptr)
    , m_documentation(nullptr)
{
    Settings* settings = lv::ViewContext::instance().settings();

    m_keymap = new KeyMap(settings->path());
    settings->addConfigFile("keymap", m_keymap);
    m_commands->setModel(new CommandsModel(m_commands, m_keymap));

    m_extensions = new Extensions(lv::ViewContext::instance().engine(), settings->path());
    settings->addConfigFile("extensions", m_extensions);

    m_engine = lv::ViewContext::instance().engine();
    QQmlEngine* engine = m_engine->engine();
    QObject* probject = engine->rootContext()->contextProperty("project").value<QObject*>();
    m_project = qobject_cast<lv::Project*>(probject);

    m_documentation = new Documentation(m_engine->packageGraph(), this);

    QObject* lk = engine->rootContext()->contextProperty("lk").value<QObject*>();
    if ( !lk ){
        qWarning("Failed to find live global object.");
        return;
    }

    QObject* windowLayerOb = lk->property("layers").value<QQmlPropertyMap*>()->property("window").value<QObject*>();

    if ( !windowLayerOb ){
        Exception e = CREATE_EXCEPTION(lv::Exception, "Workspace layer requires window layer.", Exception::toCode("~layer"));
        lv::ViewContext::instance().engine()->throwError(&e, this);
        return;
    }

    WindowLayer* windowLayer = qobject_cast<WindowLayer*>(windowLayerOb);
    QQuickWindow* window = windowLayer->window();

    m_workspace = new Workspace(m_project, this);

    connect(window, SIGNAL(closing(QQuickCloseEvent*)), this, SLOT(whenMainWindowClose()));

    setHasView(true);
}

WorkspaceLayer::~WorkspaceLayer(){
    delete m_themes;
}

void WorkspaceLayer::loadView(ViewEngine *engine, QObject *parent){

    // load default theme

    m_themes->addTheme("LiveKeys", ":/LiveKeysTheme.qml");

    loadConfigurations();

    for ( auto it = m_extensions->begin(); it != m_extensions->end(); ++it ){
        WorkspaceExtension* le = it.value();
        m_commands->add(le, le->commands());
        m_keymap->store(le->keyBindings());

        QJSValue themesArray = le->themes();

        if ( !themesArray.isUndefined() ){
            if ( !themesArray.isArray() ){
                THROW_EXCEPTION(lv::Exception, "Themes not of array type in extension: " + le->name(), Exception::toCode("~Themes"));
            }
            QJSValueIterator themeIt(themesArray);
            while ( themeIt.hasNext() ){
                themeIt.next();

                QJSValue themeObject = themeIt.value();
                if ( themeObject.isObject() )
                    THROW_EXCEPTION(lv::Exception, "Theme is not of object type in extension: " + le->name(), Exception::toCode("~Themes"));
                if ( !themeObject.hasOwnProperty("name") )
                    THROW_EXCEPTION(lv::Exception, "Theme does not have \'name\' property in exctension: " + le->name(), Exception::toCode("~Property"));
                if ( !themeObject.hasOwnProperty("path") )
                    THROW_EXCEPTION(lv::Exception, "Theme does not have \'path\' property in exctension: " + le->name(), Exception::toCode("~Property"));

                QString name = themeObject.property("name").toString();
                QString path = QString::fromStdString(it.key()) + "/'" + themeObject.property("path").toString();

                m_themes->addTheme(name, path);
            }
        }


    }

    QString path = ":/workspace.qml";

    QFile f(path);
    if ( !f.open(QFile::ReadOnly) )
        THROW_EXCEPTION(Exception, "Failed to read layer view file:" + path.toStdString(), Exception::toCode("~File"));

    QByteArray contentBytes = f.readAll();

    m_viewRoot = engine->createObject(contentBytes, parent, QUrl("qrc:/workspace.qml"));
    engine->engine()->setObjectOwnership(m_viewRoot, QQmlEngine::CppOwnership);
    if ( !m_viewRoot && engine->lastErrors().size() > 0 )
        THROW_EXCEPTION(
            Exception, ViewEngine::toErrorString(engine->lastErrors()).toStdString(), Exception::toCode("~Component")
        );

    m_projectEnvironment = m_viewRoot->property("projectEnvironment").value<QObject*>();
    m_panes              = m_viewRoot->property("panes").value<QObject*>();
    m_nextViewParent     = m_viewRoot->property("runSpace").value<QObject*>();
    QJSValue paneFactories = m_panes->property("factories").value<QJSValue>();

    m_keymap->store(0, Qt::Key_O,         lv::KeyMap::CONTROL_OR_COMMAND, "window.workspace.project.openFile");
    m_keymap->store(0, Qt::Key_Backslash, lv::KeyMap::CONTROL_OR_COMMAND, "window.workspace.project.toggleVisibility");
    m_keymap->store(0, Qt::Key_T,         lv::KeyMap::CONTROL_OR_COMMAND, "window.workspace.toggleMaximizedRuntime");
    m_keymap->store(0, Qt::Key_K,         lv::KeyMap::CONTROL_OR_COMMAND, "window.workspace.toggleNavigation");
    m_keymap->store(0, Qt::Key_L,         lv::KeyMap::CONTROL_OR_COMMAND, "window.workspace.toggleLog");
    m_keymap->store(0, Qt::Key_F1,        0,                              "window.workspace.help");
    m_keymap->store(0, Qt::Key_H,         lv::KeyMap::Alt,                "window.workspace.help");

    for ( auto it = m_extensions->begin(); it != m_extensions->end(); ++it ){
        WorkspaceExtension* le = it.value();

        QJSValue panes = le->panes();
        if ( panes.isObject() ){

            QJSValueIterator panesIt(panes);
            while ( panesIt.hasNext() ){
                panesIt.next();
                paneFactories.setProperty(panesIt.name(), panesIt.value());
            }
        }
    }

    m_panes->property("initializeStartupBox").value<QJSValue>().call();

    emit viewReady(this, m_nextViewParent);
}

QObject *WorkspaceLayer::nextViewParent(){
    return m_nextViewParent;
}

QObject *WorkspaceLayer::viewRoot(){
    return m_viewRoot;
}

QJSValue WorkspaceLayer::interceptMenu(QJSValue context){
    QJSValueList interceptorArgs;
    interceptorArgs << context;

    QJSValueList result;

    for ( auto it = m_extensions->begin(); it != m_extensions->end(); ++it ){
        WorkspaceExtension* le = it.value();
        if ( le->hasMenuInterceptor() ){
            QJSValue v = le->callMenuInterceptor(interceptorArgs);
            if ( v.isArray() ){
                QJSValueIterator vit(v);
                while ( vit.hasNext() ){
                    vit.next();
                    if ( vit.name() != "length" ){
                        result << vit.value();
                    }
                }
            }
        }
    }

    QJSValue concat = lv::ViewContext::instance().engine()->engine()->newArray(static_cast<uint32_t>(result.length()));
    quint32 index = 0;
    for ( auto it = result.begin(); it != result.end(); ++it ){
        concat.setProperty(index, *it);
        ++index;
    }

    return concat;
}

QJSValue WorkspaceLayer::interceptFile(const QString &path, int mode){
    QJSValueList interceptorArgs;
    interceptorArgs << path << mode;

    for ( auto it = m_extensions->begin(); it != m_extensions->end(); ++it ){
        WorkspaceExtension* le = it.value();
        if ( le->hasFileInterceptor() ){
            QJSValue v = le->callFileInterceptor(interceptorArgs);
            if ( v.isQObject() ){
                vlog("workspace").v() << "Found file interceptor in extension " << le->name() << ".";
                return v;
            }
        }
    }

    return QJSValue();
}

void WorkspaceLayer::addPane(QQuickItem *pane, QQuickWindow *window, const QVariantList &position){
    try{
        if ( m_workspace->currentProjectWorkspace() )
            m_workspace->currentProjectWorkspace()->whenPaneAdded(pane, window, position);
    } catch ( lv::Exception& e ){
        lv::ViewContext::instance().engine()->throwError(&e, this);
    }
}

void WorkspaceLayer::removePane(QQuickItem *pane){
    try{
        if ( m_workspace->currentProjectWorkspace() )
            m_workspace->currentProjectWorkspace()->whenPaneRemoved(pane);
    } catch ( lv::Exception& e ){
        lv::ViewContext::instance().engine()->throwError(&e, this);
    }
}

void WorkspaceLayer::addWindow(QQuickWindow *window){
    try{
        if ( m_workspace->currentProjectWorkspace() )
            m_workspace->currentProjectWorkspace()->whenWindowOpen(window);
    } catch ( lv::Exception& e ){
        lv::ViewContext::instance().engine()->throwError(&e, this);
    }
}

void WorkspaceLayer::whenMainWindowClose(){
    delete m_workspace;
    m_workspace = nullptr;
}

void WorkspaceLayer::whenProjectOpen(const QString &, ProjectWorkspace *workspace){
    QJSValue v = m_panes->property("initializePanes").value<QJSValue>();

    const MLNode& layout = workspace->currentLayout();

    QMap<QByteArray, ProjectDocument*> openDocuments;

    if ( layout.hasKey("documents") && layout["documents"].type() == MLNode::Array ){
        for ( auto it = layout["documents"].begin(); it != layout["documents"].end(); ++it ){
            QString path = QString::fromStdString((*it).asString());
            ProjectDocument* doc = m_project->openTextFile(path);
            openDocuments[Project::hashPath(path.toUtf8()).toHex()] = doc;
        }
    }

    if ( layout.hasKey("panes") && layout["panes"].size() > 0 ){

        QQmlEngine* engine = lv::ViewContext::instance().engine()->engine();

        QJSValue jsPanes;
        ml::toQml(layout["panes"], jsPanes, engine);

        QJSValue jsWindows;
        ml::toQml(layout["windows"], jsWindows, engine);

        QJSValue initialPanes = v.callWithInstance(engine->newQObject(m_panes), QJSValueList() << jsWindows << jsPanes);
        initializePanesAndWindows(workspace, initialPanes);
    }

    if ( layout.hasKey("active") && !openDocuments.isEmpty() ){
        QByteArray activeKey = QByteArray::fromStdString(layout["active"].asString());
        auto it = openDocuments.find(activeKey);
        if ( it != openDocuments.end() ){
            m_project->setActive(it.value()->file()->path());
        }
    }
}

void WorkspaceLayer::whenProjectClose(){
    QJSValue v = m_panes->property("__clearPanes").value<QJSValue>();
    v.call();
}

QString WorkspaceLayer::docsPath() const{
    return QString::fromStdString(ApplicationContext::instance().docsPath());
}

bool WorkspaceLayer::wasRecentsFileFound() const{
    return m_workspace->wasRecentsFileFound();
}

void WorkspaceLayer::initializePanes(ProjectWorkspace *workspace, QJSValue panes){
    QJSValueIterator panesIt(panes);
    while ( panesIt.hasNext() ){
        panesIt.next();
        if ( panesIt.name() != "length" ){
            QObject* pane = panesIt.value().toQObject();
            QQuickItem* paneItem = qobject_cast<QQuickItem*>(pane);
            if ( paneItem ){
                if ( paneItem->property("paneType").toString() == "splitview" ){
                    QJSValue nestedPanes = paneItem->property("panes").value<QJSValue>();
                    initializePanes(workspace, nestedPanes);
                } else {
                    workspace->whenPaneInitialized(paneItem);
                }
            }
        }
    }
}

void WorkspaceLayer::initializePanesAndWindows(ProjectWorkspace *workspace, QJSValue panesAndWindows){
    if ( panesAndWindows.property("length").toInt() == 2 ){
        QJSValue windows = panesAndWindows.property(0);
        QJSValue windowPanes = panesAndWindows.property(1);

        if ( !windows.isArray() || !windowPanes.isArray() ){
            return;
        }

        QJSValueIterator windowsIt(windows);
        if ( windowsIt.hasNext() )
            windowsIt.next(); // skip application window
        while ( windowsIt.hasNext() ){
            windowsIt.next();
            if ( windowsIt.name() != "length" ){
                QObject* object = windowsIt.value().toQObject();
                QQuickWindow* windowObject = qobject_cast<QQuickWindow*>(object);

                workspace->whenWindowInitialized(windowObject);
            }
        }

        QJSValueIterator windowPanesIt(windowPanes);
        while ( windowPanesIt.hasNext() ){
            windowPanesIt.next();
            if ( windowPanesIt.name() != "length" ){
                QJSValue panes = windowPanesIt.value();
                initializePanes(workspace, panes);
            }
        }
    }
}

void WorkspaceLayer::loadConfigurations(){
    QFile file(m_extensions->path());

    MLNode extensionConfiguration;
    MLNode tutorialsConfiguration = MLNode(MLNode::Array);
    MLNode samplesConfiguration = MLNode(MLNode::Array);

    if ( !file.exists() ){
        const MLNode& startupcfg = ApplicationContext::instance().startupConfiguration();
        if ( startupcfg.hasKey("workspace") ){
            extensionConfiguration = startupcfg["workspace"]["extensions"];
            tutorialsConfiguration = startupcfg["workspace"]["tutorials"];
            samplesConfiguration   = startupcfg["workspace"]["samples"];
        }

        if ( file.open(QIODevice::WriteOnly) ){
            std::string r;
            MLNode n(MLNode::Object);
            n["extensions"] = extensionConfiguration;
            n["samples"]    = samplesConfiguration;
            n["tutorials"]  = tutorialsConfiguration;

            ml::toJson(n, r);

            vlog("extensions").v() << "Saving extensions file to: " << m_extensions->path();

            file.write(r.c_str(), static_cast<int>(r.length()));
            file.close();
        }
    } else if ( file.open(QIODevice::ReadOnly) ){
        MLNode n;
        QByteArray fileContents = file.readAll();
        ml::fromJson(fileContents.data(), n);

        if ( n.hasKey("extensions") ){
            extensionConfiguration = n["extensions"];
        }
        if ( n.hasKey("samples") ){
            samplesConfiguration = n["samples"];
        }
        if ( n.hasKey("tutorials") ){
            tutorialsConfiguration = n["tutorials"];
        }
    }

    MLNode::ArrayType extensionArray = extensionConfiguration.asArray();

    for ( auto it = extensionArray.begin(); it != extensionArray.end(); ++it ){
        MLNode currentExt = *it;

        bool isEnabled = currentExt["enabled"].asBool();
        if ( isEnabled ){
            std::string packageName = currentExt["package"].asString();
            std::string packagePath = packageName;
            QFileInfo fpath(QString::fromStdString(packagePath));
            if ( fpath.isRelative() ){
                packagePath = ApplicationContext::instance().pluginPath() + "/" + packageName;
            }
            std::string component = currentExt["component"].asString();

            if ( Package::existsIn(packagePath) ){
                WorkspaceExtension* le = loadPackageExtension(packagePath, component);
                if ( le ){ // if package has extension
                    vlog("extensions").v() << "Loaded extension from package: " << packagePath;
                }
            }
        }
    }

    MLNode::ArrayType tutorialsArray = tutorialsConfiguration.asArray();
    for( auto it = tutorialsArray.begin(); it != tutorialsArray.end(); ++it ){
        bool isEnabled = (*it)["enabled"].asBool();
        std::string packageName = (*it)["package"].asString();
        if ( isEnabled ){
            std::string packagePath = ApplicationContext::instance().pluginPath() + "/" + packageName;
            if ( Package::existsIn(packagePath) ){
                Package::Ptr p = Package::createFromPath(packagePath);
                if ( p ){
                    //TODO #426: Store in model
//                    vlog() << "Package : " << p->name() << " tutorials label : " << p->workspaceTutorialsLabel();
//                    for ( auto section : p->workspaceTutorialsSections() ){
//                        vlog() << "Tutorial: " << section.first;
//                        vlog() << "In path: " << p->path() + "/" + section.second;
//                    }
                }
            }
        }
    }

    MLNode::ArrayType samplesArray = samplesConfiguration.asArray();
    for( auto it = samplesArray.begin(); it != samplesArray.end(); ++it ){
        bool isEnabled = (*it)["enabled"].asBool();
        std::string packageName = (*it)["package"].asString();
        if ( isEnabled ){
            std::string packagePath = ApplicationContext::instance().pluginPath() + "/" + packageName;
            if ( Package::existsIn(packagePath) ){
                Package::Ptr p = Package::createFromPath(packagePath);
                if ( p ){
                    //TODO #426: Store in model
//                    vlog() << "Package : " << p->name();
//                    for ( auto sample : p->workspaceSamples() ){
//                        vlog() << "Sample: " << sample;
//                    }
                }
            }
        }
    }
}

WorkspaceExtension *WorkspaceLayer::loadPackageExtension(const std::string &path, const std::string &component){
    Package::Ptr p = Package::createFromPath(path);
    if ( !p )
        return nullptr;
    return loadPackageExtension(p, component);
}

WorkspaceExtension *WorkspaceLayer::loadPackageExtension(const Package::Ptr &package, const std::string &componentName){
    std::string path = package->path() + "/" + componentName;
    vlog("extensions").v() << "Loading extension: " << path;

    QQmlComponent component(m_engine->engine(), QUrl::fromLocalFile(QString::fromStdString(path)), m_extensions);
    if ( component.isError() ){
        THROW_EXCEPTION(lv::Exception, "Failed to load component: " + component.errorString().toStdString(), 4);
    }

    WorkspaceExtension* le = qobject_cast<WorkspaceExtension*>(component.create());
    if ( !le ){
        THROW_EXCEPTION(lv::Exception, "Extension failed to create or cast to LiveExtension type in: " + path, 3);
    }

    le->setIdentifiers(package->name(), path);
    le->setParent(m_extensions);
    m_extensions->m_extensions.insert(le->name(), le);
    m_extensions->m_globals->insert(QString::fromStdString(le->name()), QVariant::fromValue(le->globals()));

    return le;
}

}// namespace
