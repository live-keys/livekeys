#include "projectworkspace.h"
#include "projectdocumentmodel.h"
#include "workspace.h"

#include <QFileInfo>
#include <QQuickItem>
#include <QQmlEngine>
#include <QQmlContext>
#include <QQmlProperty>
#include <QQmlPropertyMap>

#include "live/project.h"
#include "live/projectdocument.h"
#include "live/projectfile.h"
#include "live/viewcontext.h"
#include "live/viewengine.h"
#include "live/windowlayer.h"
#include "live/visuallogqt.h"
#include "live/mlnode.h"
#include "live/mlnodetojson.h"
#include "live/mlnodetoqml.h"

namespace lv{

// class ProjectWorkspace::State
// -----------------------------

class ProjectWorkspace::State{

public:
    void projectActiveChange(ProjectDocument* active);
    void documentOpen(ProjectDocument* document);
    void documentClosed(ProjectDocument* document);
    void windowRectChanged(QWindow* window, const QRect& r);
    void windowVisibilityChanged(QWindow* window, QWindow::Visibility visibility);

    void paneAdded(QQuickItem* pane, QWindow* window, const QVariantList &position);
    void paneSizeChanged(QQuickItem* pane, const QSize &r);
    void paneStateChanged(QQuickItem* pane, const QVariant& state);
    void paneMoved(QQuickItem* pane, QWindow* window, const QVariantList &position);
    void paneRemoved(QQuickItem* pane);

    MLNode currentWorkspaceLayout;
};

void ProjectWorkspace::State::projectActiveChange(ProjectDocument* document){
    if ( document )
        currentWorkspaceLayout["active"] = Project::hashPath(document->file()->path().toUtf8()).toHex().toStdString();
}

void ProjectWorkspace::State::documentOpen(ProjectDocument *document){
    std::string path = document->file()->path().toStdString();

    if ( currentWorkspaceLayout.hasKey("documents") ){

        for ( auto it = currentWorkspaceLayout["documents"].asArray().begin();
              it != currentWorkspaceLayout["documents"].asArray().end(); ++it )
        {
            if ( it->asString() == path )
                return;
        }

        currentWorkspaceLayout["documents"].append(path);

    } else {
        currentWorkspaceLayout["documents"] = {path};
    }
}

void ProjectWorkspace::State::documentClosed(ProjectDocument *document){
    std::string path = document->file()->path().toStdString();
    for ( int i = 0; i < currentWorkspaceLayout["documents"].size(); ++i ){
        if ( currentWorkspaceLayout["documents"][i].asString() == path ){
            currentWorkspaceLayout["documents"].remove(i);
        }
    }
}

void ProjectWorkspace::State::windowRectChanged(QWindow*, const QRect &r){
    if ( !currentWorkspaceLayout.hasKey("mainWindow") )
        currentWorkspaceLayout["mainWindow"] = MLNode(MLNode::Object);
    currentWorkspaceLayout["mainWindow"]["box"] = {r.x(), r.y(), r.width(), r.height()};
}

void ProjectWorkspace::State::windowVisibilityChanged(QWindow*, QWindow::Visibility visibility){
    if ( !currentWorkspaceLayout.hasKey("mainWindow") )
        currentWorkspaceLayout["mainWindow"] = MLNode(MLNode::Object);

    if ( visibility == QWindow::Windowed )
        currentWorkspaceLayout["mainWindow"]["visibility"] = "windowed";
    else if ( visibility == QWindow::FullScreen )
        currentWorkspaceLayout["mainWindow"]["visibility"] = "fullscreen";
    else if ( visibility == QWindow::Maximized )
        currentWorkspaceLayout["mainWindow"]["visibility"] = "maximized";
}

void ProjectWorkspace::State::paneAdded(QQuickItem *pane, QWindow *, const QVariantList &position){
    if ( !currentWorkspaceLayout.hasKey("panes") ){
        currentWorkspaceLayout["panes"] = MLNode(MLNode::Array);
        currentWorkspaceLayout["panes"].append(MLNode(MLNode::Array));
    }

    QString paneType = pane->property("paneType").toString();

    MLNode paneNode(MLNode::Object);
    paneNode["type"] = paneType.toStdString();
    paneNode["size"] = {pane->property("width").toInt(), pane->property("height").toInt()};
    paneNode["state"] = MLNode::Object;

    MLNode& paneLookup = currentWorkspaceLayout["panes"][0];

    QList<int> positionIndex;

    for ( int i = 0; i < position.size(); ++i ){
        int pos = position[i].toInt();
        int posIndex = pos < 0 ? -pos + 1 : pos + 1;

        positionIndex.append(pos);

        if ( posIndex > paneLookup.size() ){
            if ( i == position.size() - 1 ){ // add pane
                paneLookup.append(paneNode);
            } else { // add split
                if ( position[i + 1].toInt() < 0 ){
                    paneLookup.append({"v"});
                    paneLookup = paneLookup[paneLookup.size() - 1];
                } else {
                    paneLookup.append({"h"});
                    paneLookup = paneLookup[paneLookup.size() - 1];
                }
            }
        } else {
            if ( i == position.size() - 1 ){ // add pane
                paneLookup.asArray().insert(paneLookup.asArray().begin() + posIndex, paneNode);
            } else { // add split
                if ( position[i + 1].toInt() < 0 ){
                    paneLookup.asArray().insert(paneLookup.asArray().begin() + posIndex, {"v"});
                    paneLookup = paneLookup[posIndex];
                } else {
                    paneLookup.asArray().insert(paneLookup.asArray().begin() + posIndex, {"h"});
                    paneLookup = paneLookup[posIndex];
                }
            }
        }
    }

    QStringList positionIndexOutput;
    for ( auto item : positionIndex )
        positionIndexOutput.append(QString::number(item));

    vlog("appdata").v() << "Pane " << paneType << " added to layout at [" << positionIndexOutput.join(",") << "]";
}

void ProjectWorkspace::State::paneSizeChanged(QQuickItem *pane, const QSize &size){
    QVariantList location = pane->property("paneLocation").toList();

    MLNode* paneLookup = &currentWorkspaceLayout["panes"][0];

    for ( auto it = location.begin(); it != location.end(); ++it ){
        if ( paneLookup->type() != MLNode::Array )
            return;

        int position = it->toInt();
        int pos = position < 0 ? -position : position;

        int posToIndex = pos + 1; // account for 'v' and 'h'

        if ( posToIndex >= paneLookup->size() )
            return;

        paneLookup = &(*paneLookup)[posToIndex];
    }

    (*paneLookup)["size"] = {size.width(), size.height()};
}

void ProjectWorkspace::State::paneStateChanged(QQuickItem *pane, const QVariant &v){
    QVariantList location = pane->property("paneLocation").toList();

    MLNode* paneLookup = &currentWorkspaceLayout["panes"][0];

    for ( auto it = location.begin(); it != location.end(); ++it ){
        if ( paneLookup->type() != MLNode::Array )
            return;

        int position = it->toInt();
        int pos = position < 0 ? -position : position;

        int posToIndex = pos + 1; // account for 'v' and 'h'

        if ( posToIndex >= paneLookup->size() )
            return;

        paneLookup = &(*paneLookup)[posToIndex];
    }

    QJSValue ps = v.value<QJSValue>();
    if ( ps.hasOwnProperty("document") ){
        ProjectDocument* doc = static_cast<ProjectDocument*>(ps.property("document").toQObject());
        if ( doc )
            ps.setProperty("document", doc->file()->hashPath());
    }

    MLNode paneStateNode;
    ml::fromQml(ps, paneStateNode);

    (*paneLookup)["state"] = paneStateNode;
}

void ProjectWorkspace::State::paneMoved(QQuickItem *, QWindow *, const QVariantList &){
    //TODO
}

void ProjectWorkspace::State::paneRemoved(QQuickItem *pane){
    QVariantList location = pane->property("paneLocation").toList();

    MLNode* paneLookup = &currentWorkspaceLayout["panes"][0];

    for ( int i = 0; i < location.size(); ++i ){
        if ( paneLookup->type() != MLNode::Array )
            return;

        int position = location[i].toInt();
        int pos = position < 0 ? -position : position;

        int posToIndex = pos + 1; // account for 'v' and 'h'

        if ( posToIndex >= paneLookup->size() )
            return;

        if ( i == location.size() - 1 ){
            paneLookup->asArray().erase(paneLookup->asArray().begin() + posToIndex);
            break;
        }

        paneLookup = &(*paneLookup)[posToIndex];
    }
}

// class ProjectWorkspace
// ----------------------

ProjectWorkspace::ProjectWorkspace(Project* project, QObject *parent)
    : QObject(parent)
    , m_project(project)
    , m_window(nullptr)
    , m_state(new ProjectWorkspace::State)
{
    connect(m_project, &Project::activeChanged,  this, &ProjectWorkspace::whenProjectActiveChange);
    connect(m_project, &Project::documentOpened, this, &ProjectWorkspace::whenDocumentOpen);
    connect(m_project->documentModel(), &ProjectDocumentModel::aboutToClose, this, &ProjectWorkspace::whenDocumentClose);

    QQmlEngine* engine = lv::ViewContext::instance().engine()->engine();

    QObject* lk = engine->rootContext()->contextProperty("livecv").value<QObject*>();
    if ( !lk ){
        qWarning("Failed to find live global object.");
        return;
    }

    QObject* windowLayerOb = lk->property("layers").value<QQmlPropertyMap*>()->property("window").value<QObject*>();
    WindowLayer* windowLayer = qobject_cast<WindowLayer*>(windowLayerOb);

    m_window = windowLayer->window();

    connect(m_window, &QQuickWindow::xChanged, this, &ProjectWorkspace::whenWindowRectChanged);
    connect(m_window, &QQuickWindow::yChanged, this, &ProjectWorkspace::whenWindowRectChanged);
    connect(m_window, &QQuickWindow::widthChanged, this, &ProjectWorkspace::whenWindowRectChanged);
    connect(m_window, &QQuickWindow::heightChanged, this, &ProjectWorkspace::whenWindowRectChanged);
    connect(m_window, &QQuickWindow::visibilityChanged, this, &ProjectWorkspace::whenWindowVisibilityChanged);
}

ProjectWorkspace::~ProjectWorkspace(){
    save();
    delete m_state;
    for ( auto it = m_watchers.begin(); it != m_watchers.end(); ++it ){
        delete *it;
    }
}

void ProjectWorkspace::save() const{
    if ( m_currentWorkspaceId.isEmpty() )
        return;

    Workspace::absoluteDir("cache");
    QString workspaceDir = Workspace::absoluteDir("cache/" + m_currentWorkspaceId);

    QFile layoutFile(workspaceDir + "/layout.json");

    if ( layoutFile.open(QIODevice::WriteOnly) ){
        try{
            vlog("appdata").v() << "Saving workspace layout to file: " << layoutFile.fileName();

            std::string result;
            ml::toJson(m_state->currentWorkspaceLayout, result);

            layoutFile.write(result.c_str(), result.size());

        } catch ( Exception& e ){
            vlog().w() << "Failed to save file \'" << layoutFile.fileName() << "\': " + e.message();
        }
    }
}

const MLNode &ProjectWorkspace::currentLayout() const{
    return m_state->currentWorkspaceLayout;
}

ProjectWorkspace *ProjectWorkspace::create(Project *project, QObject *parent){

    ProjectWorkspace* pw = new ProjectWorkspace(project, parent);
    pw->initialize();

    return pw;
}

void ProjectWorkspace::initialize(){
    QString path = m_project->dir();
    QFileInfo pathinfo(path);

    if ( path.isEmpty() || !pathinfo.exists() )
        initializeDefaults();
    else
        initializeFromId();

    vlog("appdata").v() << "Initialized workspace: " << m_currentWorkspaceId;
}

void ProjectWorkspace::initializeFromId(){
    QString path = m_project->dir();

    m_currentWorkspaceId = "";
    m_state->currentWorkspaceLayout = MLNode(MLNode::Object);

    m_currentWorkspaceId = Project::hashPath(path.toUtf8()).toHex();

    vlog("appdata").v() << "Loading workspace: " << path << "(" << m_currentWorkspaceId << ")";

    Workspace::absoluteDir("cache");
    QString workspaceDir = Workspace::absoluteDir("cache/" + m_currentWorkspaceId);

    QFile layoutFile(workspaceDir + "/layout.json");

    if ( layoutFile.exists() && layoutFile.open(QIODevice::ReadOnly) ){
        try{
            vlog("appdata").v() << "Loading workspace layout from file: " << layoutFile.fileName();

            QByteArray result = layoutFile.readAll();

            m_state->currentWorkspaceLayout = MLNode();
            ml::fromJson(result.constData(), m_state->currentWorkspaceLayout);

        } catch ( Exception& e ){
            vlog().w() << "Failed to parse file \'" << layoutFile.fileName() << "\': " + e.message();
        }
    } else {
        MLNode projectViewPane = MLNode(MLNode::Object);
        projectViewPane["type"] = "projectFileSystem";
        projectViewPane["size"] = {250, 250};

        m_state->currentWorkspaceLayout["panes"] = MLNode(MLNode::Array);
        m_state->currentWorkspaceLayout["panes"].append(MLNode::Array); // main window
        m_state->currentWorkspaceLayout["panes"][0].append(MLNode("h"));
        m_state->currentWorkspaceLayout["panes"][0].append(projectViewPane);
    }

    //TODO: Delete deprecated workspaces
    // * use a trigger with timer when any project is opened
    // * use separate thread
}

void ProjectWorkspace::initializeDefaults(){

    vlog("appdata").v() << "Creating workspace: " << (m_project->dir().isEmpty() ? "untitled" : m_project->dir());

    m_currentWorkspaceId = "";
    m_state->currentWorkspaceLayout = MLNode(MLNode::Object);

    QString path = m_project->dir();
    if ( !path.isEmpty() )
        m_currentWorkspaceId = Project::hashPath(path.toUtf8()).toHex();

    MLNode projectViewPane = MLNode(MLNode::Object);
    projectViewPane["type"] = "projectFileSystem";
    projectViewPane["size"] = {250, 250};

    m_state->currentWorkspaceLayout["panes"] = MLNode(MLNode::Array);
    m_state->currentWorkspaceLayout["panes"].append(MLNode::Array); // main window
    m_state->currentWorkspaceLayout["panes"][0].append(MLNode("h"));
    m_state->currentWorkspaceLayout["panes"][0].append(projectViewPane);
}


void ProjectWorkspace::whenProjectActiveChange(ProjectDocument *document){
    m_state->projectActiveChange(document);
    emit projectActiveChange(document);
}

void ProjectWorkspace::whenDocumentOpen(ProjectDocument *document){
    connect(document->textDocument(), &QTextDocument::contentsChange, this, &ProjectWorkspace::whenDocumentContentsChanged);
    connect(document, &ProjectDocument::isMonitoredChanged, this, &ProjectWorkspace::whenDocumentIsMonitoredChanged);
    connect(document, &ProjectDocument::saved, this, &ProjectWorkspace::whenDocumentSaved);

    m_state->documentOpen(document);
    emit documentOpen(document);
}

void ProjectWorkspace::whenDocumentClose(ProjectDocument *document){
    m_state->documentClosed(document);
    emit documentClose(document);
}

void ProjectWorkspace::whenDocumentContentsChanged(int p, int a, int r){
    QTextDocument* textDocument = static_cast<QTextDocument*>(sender());
    ProjectDocument* document = static_cast<ProjectDocument*>(textDocument->parent());

    QString content;
    if ( a > 0 ){
        QTextCursor cursor(document->textDocument());
        cursor.setPosition(p);
        cursor.setPosition(p + a, QTextCursor::KeepAnchor);
        content = cursor.selectedText();
    }
    emit documentContentsChange(document, p, p + r, content);
}

void ProjectWorkspace::whenDocumentIsMonitoredChanged(){
    ProjectDocument* document = static_cast<ProjectDocument*>(sender());
    emit documentMonitorStateChange(document, document->isMonitored());
}

void ProjectWorkspace::whenDocumentSaved(){
    ProjectDocument* document = static_cast<ProjectDocument*>(sender());
    emit documentSave(document);
}

void ProjectWorkspace::whenWindowRectChanged(){
    m_state->windowRectChanged(m_window, QRect(m_window->x(), m_window->y(), m_window->width(), m_window->height()));
    emit windowRectChanged(m_window, QRect(m_window->x(), m_window->y(), m_window->width(), m_window->height()));
}

void ProjectWorkspace::whenWindowVisibilityChanged(){
    m_state->windowVisibilityChanged(m_window, m_window->visibility());
    emit windowVisibilityChanged(m_window, m_window->visibility());
}

void ProjectWorkspace::whenPaneInitialized(QQuickItem *pane, QWindow *){
    connect(pane, &QQuickItem::widthChanged, this, [this, pane](){
        emit paneSizeChanged(pane, QSize(pane->width(), pane->height()));
        m_state->paneSizeChanged(pane, QSize(pane->width(), pane->height()));
    });
    connect(pane, &QQuickItem::heightChanged, this, [this, pane](){
        emit paneSizeChanged(pane, QSize(pane->width(), pane->height()));
        m_state->paneSizeChanged(pane, QSize(pane->width(), pane->height()));
    });

    QmlPropertyWatcher* paneStateWatcher = new QmlPropertyWatcher(pane, "paneState");
    if ( paneStateWatcher->isValid() ){
        paneStateWatcher->onChange([this](const QmlPropertyWatcher& w){
            QQuickItem* p = static_cast<QQuickItem*>(w.object());
            QVariant v = w.read();
            m_state->paneStateChanged(p, v);
            emit paneStateChanged(p, v);
        });
        m_watchers.append(paneStateWatcher);
    } else {
        delete paneStateWatcher;
    }
}

void ProjectWorkspace::whenPaneAdded(QQuickItem *pane, QWindow *window, const QVariantList &position){
    connect(pane, &QQuickItem::widthChanged, this, [this, pane](){
        emit paneSizeChanged(pane, QSize(pane->width(), pane->height()));
        m_state->paneSizeChanged(pane, QSize(pane->width(), pane->height()));
    });
    connect(pane, &QQuickItem::heightChanged, this, [this, pane](){
        emit paneSizeChanged(pane, QSize(pane->width(), pane->height()));
        m_state->paneSizeChanged(pane, QSize(pane->width(), pane->height()));
    });

    QmlPropertyWatcher* paneStateWatcher = new QmlPropertyWatcher(pane, "paneState");
    if ( paneStateWatcher->isValid() ){
        paneStateWatcher->onChange([this](const QmlPropertyWatcher& w){
            emit paneStateChanged(static_cast<QQuickItem*>(w.object()), w.read());
        });
        m_watchers.append(paneStateWatcher);
    } else {
        delete paneStateWatcher;
    }

    m_state->paneAdded(pane, window, position);

    emit paneAdded(pane, window, position);
}

void ProjectWorkspace::whenPaneMoved(QQuickItem *pane, QWindow *window, const QVariantList &position){
    m_state->paneMoved(pane, window, position);
    emit paneMoved(pane, window, position);
}

void ProjectWorkspace::whenPaneRemoved(QQuickItem *pane){
    for ( auto it = m_watchers.begin(); it != m_watchers.end(); ++it ){
        QmlPropertyWatcher* qpw = *it;
        if ( qpw->object() == pane ){
            delete qpw;
            m_watchers.erase(it);
            break;
        }
    }

    m_state->paneRemoved(pane);
    emit paneRemoved(pane);
}

}// namespace
