#include "workspacelayer.h"
#include "live/visuallog.h"
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

#include "workspace.h"
#include "projectworkspace.h"

#include <QFile>
#include <QUrl>

#include <QQuickItem>
#include <QQmlEngine>
#include <QQmlContext>
#include <QQmlPropertyMap>
#include <QJSValueIterator>
#include <QJSValue>

namespace lv{

WorkspaceLayer::WorkspaceLayer(QObject *parent)
    : Layer(parent)
    , m_projectEnvironment(nullptr)
    , m_panes(nullptr)
    , m_commands(new Commands)
    , m_keymap(nullptr)
    , m_themes(new ThemeContainer("workspace", this))
    , m_project(nullptr)
    , m_extensions(nullptr)
{
    Settings* settings = lv::ViewContext::instance().settings();

    m_keymap = new KeyMap(settings->path());
    settings->addConfigFile("keymap", m_keymap);
    m_commands->setModel(new CommandsModel(m_commands, m_keymap));

    m_extensions = new Extensions(lv::ViewContext::instance().engine(), settings->path());
    settings->addConfigFile("extensions", m_extensions);

    QQmlEngine* engine = lv::ViewContext::instance().engine()->engine();
    QObject* probject = engine->rootContext()->contextProperty("project").value<QObject*>();
    m_project = qobject_cast<lv::Project*>(probject);

    QObject* lk = engine->rootContext()->contextProperty("livecv").value<QObject*>();
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

    m_extensions->loadExtensions();

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

    QObject* layerObj = engine->createObject(contentBytes, parent, QUrl("qrc:/workspace.qml"));
    if ( !layerObj && engine->lastErrors().size() > 0 )
        THROW_EXCEPTION(
            Exception, ViewEngine::toErrorString(engine->lastErrors()).toStdString(), Exception::toCode("~Component")
        );

    m_projectEnvironment = layerObj->property("projectEnvironment").value<QObject*>();
    m_panes              = layerObj->property("panes").value<QObject*>();
    m_nextViewParent     = layerObj->property("runSpace").value<QObject*>();
    QJSValue paneFactories = m_panes->property("factories").value<QJSValue>();

    m_keymap->store(0, Qt::Key_O,         lv::KeyMap::CONTROL_OR_COMMAND, "window.workspace.project.openFile");
    m_keymap->store(0, Qt::Key_Backslash, lv::KeyMap::CONTROL_OR_COMMAND, "window.workspace.project.toggleVisibility");
    m_keymap->store(0, Qt::Key_T,         lv::KeyMap::CONTROL_OR_COMMAND, "window.workspace.toggleMaximizedRuntime");
    m_keymap->store(0, Qt::Key_K,         lv::KeyMap::CONTROL_OR_COMMAND, "window.workspace.toggleNavigation");
    m_keymap->store(0, Qt::Key_L,         lv::KeyMap::CONTROL_OR_COMMAND, "window.workspace.toggleLog");

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

    emit viewReady(this, m_nextViewParent);
}

QObject *WorkspaceLayer::nextViewParent(){
    return m_nextViewParent;
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

    QJSValue concat = lv::ViewContext::instance().engine()->engine()->newArray(result.length());
    int index = 0;
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
            openDocuments[Project::hashPath(path.toUtf8()).toHex()] = m_project->openFile(path);
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

}// namespace
