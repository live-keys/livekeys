#include "workspacelayer.h"
#include "live/visuallog.h"
#include "live/viewengine.h"
#include "live/exception.h"
#include "live/viewcontext.h"
#include "live/settings.h"
#include "live/projectfile.h"
#include "live/liveextension.h"
#include "live/project.h"
#include "live/windowlayer.h"

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
    WindowLayer* windowLayer = qobject_cast<WindowLayer*>(windowLayerOb);
    QQuickWindow* window = windowLayer->window();

    m_workspace = new Workspace(m_project, this);

    connect(window, SIGNAL(closing(QQuickCloseEvent*)), this, SLOT(whenWindowClose()));

    setHasView(true);
}

WorkspaceLayer::~WorkspaceLayer(){
}

void WorkspaceLayer::loadView(ViewEngine *engine, QObject *parent){
    m_extensions->loadExtensions();

    for ( auto it = m_extensions->begin(); it != m_extensions->end(); ++it ){
        LiveExtension* le = it.value();
        m_commands->add(le, le->commands());
        m_keymap->store(le->keyBindings());
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
        LiveExtension* le = it.value();

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
        LiveExtension* le = it.value();
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

void WorkspaceLayer::addPane(QQuickItem *pane, QWindow *window, const QVariantList &position){
    if ( m_workspace->currentProjectWorkspace() )
        m_workspace->currentProjectWorkspace()->whenPaneAdded(pane, window, position);
}

void WorkspaceLayer::movePane(QQuickItem *pane, QWindow *window, const QVariantList &position){
    if ( m_workspace->currentProjectWorkspace() )
        m_workspace->currentProjectWorkspace()->whenPaneMoved(pane, window, position);
}

void WorkspaceLayer::removePane(QQuickItem *pane){
    if ( m_workspace->currentProjectWorkspace() )
        m_workspace->currentProjectWorkspace()->whenPaneRemoved(pane);
}

void WorkspaceLayer::whenWindowClose(){
    delete m_workspace;
    m_workspace = nullptr;
}

void WorkspaceLayer::whenProjectOpen(const QString &, ProjectWorkspace *workspace){
    QJSValue v = m_panes->property("createPane").value<QJSValue>();

    const MLNode& layout = workspace->currentLayout();

    QMap<QByteArray, ProjectDocument*> openDocuments;

    if ( layout.hasKey("documents") && layout["documents"].type() == MLNode::Array ){
        for ( auto it = layout["documents"].begin(); it != layout["documents"].end(); ++it ){
            QString path = QString::fromStdString((*it).asString());
            openDocuments[Project::hashPath(path.toUtf8()).toHex()] = m_project->openFile(path);
        }
    }

    if ( layout.hasKey("panes") && layout["panes"].size() > 0 ){
        const MLNode& panes = layout["panes"][0];

        int orientation = panes[0].asString() == "h" ? 1 : -1;

        const MLNode::ArrayType& paneItems = panes.asArray();

        for ( size_t i = 1; i < paneItems.size(); ++i ){

            const MLNode& pane = paneItems[i];

            QJSValue position = lv::ViewContext::instance().engine()->engine()->newArray(1);
            position.setProperty(0, (int)(i - 1) * orientation);

            QJSValue size = lv::ViewContext::instance().engine()->engine()->newArray(1);
            size.setProperty(0, pane["size"][0].asInt());
            size.setProperty(1, pane["size"][1].asInt());

            QJSValueList vlist;
            vlist.append(QString::fromStdString(pane["type"].asString()));
            vlist.append(QJSValue());
            vlist.append(position);
            vlist.append(size);

            QJSValue jsPane = v.call(vlist);
            QObject* objPane = jsPane.toQObject();
            QQuickItem* itemPane = qobject_cast<QQuickItem*>(objPane);

            workspace->whenPaneInitialized(itemPane, nullptr);

            QJSValue paneStateInitializer = itemPane->property("paneInitialize").value<QJSValue>();
            if ( pane.hasKey("state") && paneStateInitializer.isCallable() ){
                QJSValueList paneInitializerArgs;

                QJSValue jsState = lv::ViewContext::instance().engine()->engine()->newObject();

                MLNode mlState = pane["state"];
                if ( mlState.hasKey("document") ){

                    QByteArray doc = QByteArray::fromStdString(mlState["document"].asString());
                    auto it = openDocuments.find(doc);
                    if ( it != openDocuments.end() ){
                        ProjectDocument* doc = it.value();

                        QJSValue documentState = lv::ViewContext::instance().engine()->engine()->newQObject(doc);
                        jsState.setProperty("document", documentState);
                    }

                    mlState.remove("document");
                }

                paneInitializerArgs << jsState;

                paneStateInitializer.call(paneInitializerArgs);
            }

            vlog("appdata").v() << "Initialized pane: " << pane.toString();
        }
    }

    if ( layout.hasKey("active") && !openDocuments.isEmpty() ){
        QByteArray activeKey = QByteArray::fromStdString(layout["active"].asString());
        auto it = openDocuments.find(activeKey);
        if ( it != openDocuments.end() ){
            m_project->setActive(it.value()->file());
        }
    }
}

void WorkspaceLayer::whenProjectClose(ProjectWorkspace *){
    QJSValue v = m_panes->property("clearPanes").value<QJSValue>();
    v.call();
}

}// namespace
