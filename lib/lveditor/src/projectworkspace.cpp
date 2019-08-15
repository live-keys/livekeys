#include "projectworkspace.h"
#include "projectdocumentmodel.h"
#include "workspace.h"

#include <QJSValue>
#include <QJSValueIterator>
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

    void windowOpen(QQuickWindow* window);
    void windowClose(QQuickWindow* window);
    void windowRectChanged(QQuickWindow* window, const QRect& r);
    void windowVisibilityChanged(QQuickWindow* window, QWindow::Visibility visibility);

    void paneAdded(QQuickItem* pane, QQuickWindow* window, const QVariantList &position);
    void paneSizeChanged(QQuickItem* pane, const QSize &r);
    void paneStateChanged(QQuickItem* pane, const QVariant& state);
    void paneRemoved(QQuickItem* pane);

    MLNode currentWorkspaceLayout;

    QList<QQuickWindow*> windows;

private:
    void setPaneState(MLNode& pane, const QVariant& state);
};

void ProjectWorkspace::State::projectActiveChange(ProjectDocument* document){
    if ( document )
        currentWorkspaceLayout["active"] = Project::hashPath(document->file()->path().toUtf8()).toHex().toStdString();
}

void ProjectWorkspace::State::documentOpen(ProjectDocument *document){
    std::string path = document->file()->path().toStdString();

    if ( currentWorkspaceLayout.hasKey("documents") && currentWorkspaceLayout["documents"].type() == MLNode::Array ){

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

void ProjectWorkspace::State::paneAdded(QQuickItem *pane, QQuickWindow *window, const QVariantList &position){
    int windowIndex = windows.indexOf(window);
    if ( windowIndex < 0 )
        THROW_EXCEPTION(Exception, "Unknown window index", Exception::toCode("~Window"));

    if ( !currentWorkspaceLayout.hasKey("panes") ){
        currentWorkspaceLayout["panes"] = MLNode(MLNode::Array);
        for ( int i = 0; i < windows.size(); ++i ){
            MLNode windowPaneConfig = MLNode(MLNode::Array);
            windowPaneConfig.append("v");
            currentWorkspaceLayout["panes"].append(windowPaneConfig);
        }
    }

    QString paneType = pane->property("paneType").toString();

    MLNode paneNode(MLNode::Object);
    paneNode["type"] = paneType.toStdString();
    paneNode["size"] = {pane->property("width").toInt(), pane->property("height").toInt()};
    paneNode["state"] = MLNode::Object;

    QVariant paneState = pane->property("paneState");
    if ( paneState.isValid() )
        setPaneState(paneNode, pane->property("paneState"));

    MLNode* paneLookup = &currentWorkspaceLayout["panes"][windowIndex];

    QList<int> positionIndex;

    if ( position.isEmpty() )
        THROW_EXCEPTION(lv::Exception, "Empty position given.", Exception::toCode("~array"));

    for ( int i = 0; i < position.size() - 1; ++i ){

        int pos = position[i].toInt();
        int posIndex = pos < 0 ? -pos : pos;
        positionIndex.append(pos);

        int nextPos = position[i + 1].toInt();

        MLNode* currentNode = &(*paneLookup)[posIndex];
        if ( currentNode->type() != MLNode::Array ){ // swap node

            MLNode currentNodeCopy = *currentNode;
            (*paneLookup)[posIndex] = MLNode(MLNode::Array);

            // adjust new width/height
            if ( currentNodeCopy.hasKey("size") &&
                 currentNodeCopy["size"].type() == MLNode::Array &&
                 currentNodeCopy["size"].size() == 2 ){

                int sizeTypeIndex = nextPos < 0 ? 1 : 0;
                currentNodeCopy["size"][sizeTypeIndex] = currentNodeCopy["size"][sizeTypeIndex].asInt() / 2;
            }

            paneLookup = &(*paneLookup)[posIndex];

            paneLookup->append(nextPos < 0 ? "v" : "h");
            paneLookup->append(currentNodeCopy);

            vlog("appdata").v() << "Pane " + currentNodeCopy["type"].asString() <<
                                   " swapped with " << (nextPos < 0 ? "v" : "h") << "splitter.";
        } else {
            paneLookup = currentNode;
        }
    }

    int pos = position.last().toInt();
    int posIndex = pos < 0 ? -pos : pos;
    positionIndex.append(pos);

    paneLookup->asArray().insert(paneLookup->asArray().begin() + posIndex, paneNode);

    QStringList positionIndexOutput;
    for ( auto item : positionIndex )
        positionIndexOutput.append(QString::number(item));

    vlog("appdata").v() << "Pane " << paneType << " added to layout at [" << windowIndex << "][" << positionIndexOutput.join(",") << "]";
}

void ProjectWorkspace::State::paneSizeChanged(QQuickItem *pane, const QSize &size){
    QJSValue positioning = pane->property("splitterHierarchyPositioning").value<QJSValue>();
    QJSValue windowGetter = pane->property("paneWindow").value<QJSValue>();

    QJSValue callInstance = lv::ViewContext::instance().engine()->engine()->newQObject(pane);
    QJSValue location = positioning.callWithInstance(callInstance);
    QObject* windowObject = windowGetter.callWithInstance(callInstance).toQObject();
    QQuickWindow* window = qobject_cast<QQuickWindow*>(windowObject);
    int windowIndex = windows.indexOf(window);

    if ( windowIndex < 0 )
        THROW_EXCEPTION(Exception, "Invalid window index", Exception::toCode("~window"));

    QList<int> positionIndex;

    QJSValueIterator locationIt(location);
    while ( locationIt.hasNext() ){
        locationIt.next();
        if ( locationIt.name() != "length" ){
            positionIndex.append(locationIt.value().toInt());
        }
    }

    MLNode* paneLookup = &currentWorkspaceLayout["panes"][windowIndex];
    for ( int i = 0; i < positionIndex.size(); ++i ){
        if ( paneLookup->type() != MLNode::Array )
            return;

        int position = positionIndex[i];
        int pos = position < 0 ? -position : position;

        if ( pos >= paneLookup->size() )
            return;

        paneLookup = &(*paneLookup)[pos];
    }

    (*paneLookup)["size"] = {size.width(), size.height()};
}

void ProjectWorkspace::State::paneStateChanged(QQuickItem *pane, const QVariant &v){
    QJSValue positioning = pane->property("splitterHierarchyPositioning").value<QJSValue>();
    QJSValue windowGetter = pane->property("paneWindow").value<QJSValue>();

    QJSValue callInstance = lv::ViewContext::instance().engine()->engine()->newQObject(pane);
    QJSValue location = positioning.callWithInstance(callInstance);
    QObject* windowObject = windowGetter.callWithInstance(callInstance).toQObject();
    QQuickWindow* window = qobject_cast<QQuickWindow*>(windowObject);
    int windowIndex = windows.indexOf(window);
    if ( windowIndex < 0 )
        THROW_EXCEPTION(Exception, "Invalid window index", Exception::toCode("~window"));

    QList<int> positionIndex;

    QJSValueIterator locationIt(location);
    while ( locationIt.hasNext() ){
        locationIt.next();
        if ( locationIt.name() != "length" ){
            positionIndex.append(locationIt.value().toInt());
        }
    }

    MLNode* paneLookup = &currentWorkspaceLayout["panes"][windowIndex];
    for ( int i = 0; i < positionIndex.size(); ++i ){
        if ( paneLookup->type() != MLNode::Array )
            return;

        int position = positionIndex[i];
        int pos = position < 0 ? -position : position;

        if ( pos >= paneLookup->size() )
            return;

        paneLookup = &(*paneLookup)[pos];
    }

    setPaneState(*paneLookup, v);
}

void ProjectWorkspace::State::paneRemoved(QQuickItem *pane){
    QJSValue positioning = pane->property("splitterHierarchyPositioning").value<QJSValue>();
    QJSValue windowGetter = pane->property("paneWindow").value<QJSValue>();

    QJSValue callInstance = lv::ViewContext::instance().engine()->engine()->newQObject(pane);
    QJSValue location = positioning.callWithInstance(callInstance);
    QObject* windowObject = windowGetter.callWithInstance(callInstance).toQObject();
    QQuickWindow* window = qobject_cast<QQuickWindow*>(windowObject);
    int windowIndex = windows.indexOf(window);
    if ( windowIndex < 0 )
        THROW_EXCEPTION(Exception, "Invalid window index", Exception::toCode("~window"));

    QList<int> positionIndex;

    QJSValueIterator locationIt(location);
    while ( locationIt.hasNext() ){
        locationIt.next();
        if ( locationIt.name() != "length" ){
            positionIndex.append(locationIt.value().toInt());
        }
    }

    QString paneType = pane->property("paneType").toString();

    MLNode* paneLookup = &currentWorkspaceLayout["panes"][windowIndex];

    for ( int i = 0; i < positionIndex.size(); ++i ){
        if ( paneLookup->type() != MLNode::Array )
            return;

        int position = positionIndex[i];
        int pos = position < 0 ? -position : position;

        if ( pos >= paneLookup->size() )
            return;

        if ( i == positionIndex.size() - 1 ){
            paneLookup->asArray().erase(paneLookup->asArray().begin() + pos);
            break;
        }

        paneLookup = &(*paneLookup)[pos];
    }

    QStringList positionIndexOutput;
    for ( auto item : positionIndex )
        positionIndexOutput.append(QString::number(item));

    vlog("appdata").v() << "Pane " << paneType << " removed from layout at [" << positionIndexOutput.join(",") << "]";
}

void ProjectWorkspace::State::windowOpen(QQuickWindow *window){
    windows.append(window);

    if ( !currentWorkspaceLayout.hasKey("windows") ){
        currentWorkspaceLayout["windows"] = MLNode(MLNode::Array);
    }

    MLNode windowObject(MLNode::Object);
    windowObject["box"] = {window->x(), window->y(), window->width(), window->height()};
    windowObject["visibility"] = window->visibility() == QWindow::FullScreen
            ? "fullscreen"
            : (window->visibility() == QWindow::Maximized ? "maximized" : "windowed");

    currentWorkspaceLayout["windows"].append(windowObject);

    MLNode windowPaneConfig = MLNode(MLNode::Array);
    windowPaneConfig.append("v");
    currentWorkspaceLayout["panes"].append(windowPaneConfig);
}

void ProjectWorkspace::State::windowClose(QQuickWindow *window){
    int index = windows.indexOf(window);

    currentWorkspaceLayout["windows"].remove(index);
    currentWorkspaceLayout["panes"].remove(index);
}

void ProjectWorkspace::State::windowRectChanged(QQuickWindow *window, const QRect &r){
    int index = windows.indexOf(window);

    currentWorkspaceLayout["windows"][index]["box"] = {r.x(), r.y(), r.width(), r.height()};
}

void ProjectWorkspace::State::windowVisibilityChanged(QQuickWindow *window, QWindow::Visibility visibility){
    int index = windows.indexOf(window);

    currentWorkspaceLayout["window"][index]["visibility"] = visibility == QWindow::FullScreen
            ? "fullscreen"
            : (visibility == QWindow::Maximized ? "maximized" : "windowed");
}

void ProjectWorkspace::State::setPaneState(MLNode &pane, const QVariant &state){
    QJSValue ps = state.value<QJSValue>();
    if ( ps.hasOwnProperty("document") ){
        ProjectDocument* doc = static_cast<ProjectDocument*>(ps.property("document").toQObject());
        if ( doc )
            ps.setProperty("document", doc->file()->hashPath());
    }

    MLNode paneStateNode;
    ml::fromQml(ps, paneStateNode);

    pane["state"] = paneStateNode;
}

// class ProjectWorkspace
// ----------------------

ProjectWorkspace::ProjectWorkspace(Project* project, QObject *parent)
    : QObject(parent)
    , m_project(project)
    , m_state(new ProjectWorkspace::State)
{
    connect(m_project, &Project::aboutToClose, this, &ProjectWorkspace::whenAboutToClose);
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

    QQuickWindow* window = windowLayer->window();

    connect(window, &QQuickWindow::xChanged, this, &ProjectWorkspace::whenWindowRectChanged);
    connect(window, &QQuickWindow::yChanged, this, &ProjectWorkspace::whenWindowRectChanged);
    connect(window, &QQuickWindow::widthChanged, this, &ProjectWorkspace::whenWindowRectChanged);
    connect(window, &QQuickWindow::heightChanged, this, &ProjectWorkspace::whenWindowRectChanged);
    connect(window, &QQuickWindow::visibilityChanged, this, &ProjectWorkspace::whenWindowVisibilityChanged);

    m_state->windows.append(window);
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

void ProjectWorkspace::createLayoutNodes(){

    m_state->currentWorkspaceLayout["panes"] = MLNode(MLNode::Array);

    // initialize main window

    if ( !m_state->currentWorkspaceLayout.hasKey("windows") ){
        m_state->currentWorkspaceLayout["windows"] = MLNode(MLNode::Array);
    }

    for ( auto it = m_state->windows.begin(); it != m_state->windows.end(); ++it ){
        QQuickWindow* w = *it;

        MLNode windowOb = MLNode(MLNode::Object);
        windowOb["box"] = {w->x(), w->y(), w->width(), w->height()};
        windowOb["visibility"] = w->visibility() == QWindow::FullScreen
                ? "fullscreen"
                : (w->visibility() == QWindow::Maximized ? "maximized" : "windowed");

        m_state->currentWorkspaceLayout["windows"].append(windowOb);

        m_state->currentWorkspaceLayout["panes"].append(MLNode(MLNode::Array)); // initialize window panes
    }

    // initialize panes

    MLNode projectViewPane = MLNode(MLNode::Object);
    projectViewPane["type"] = "projectFileSystem";
    projectViewPane["size"] = {250, 250};

    MLNode viewerPane = MLNode(MLNode::Object);
    viewerPane["type"] = "viewer";
    viewerPane["size"] = {250, 250};

    m_state->currentWorkspaceLayout["panes"][0].append(MLNode("v"));
    m_state->currentWorkspaceLayout["panes"][0].append(MLNode(MLNode::Array));
    m_state->currentWorkspaceLayout["panes"][0][1].append(MLNode("h"));
    m_state->currentWorkspaceLayout["panes"][0][1].append(projectViewPane);
    m_state->currentWorkspaceLayout["panes"][0][1].append(viewerPane);
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
        createLayoutNodes();
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

    createLayoutNodes();
}


void ProjectWorkspace::whenProjectActiveChange(ProjectDocument *document){
    m_state->projectActiveChange(document);
    emit projectActiveChange(document);
}

void ProjectWorkspace::whenAboutToClose(){
    disconnect(m_project, &Project::activeChanged,  this, &ProjectWorkspace::whenProjectActiveChange);
    disconnect(m_project, &Project::documentOpened, this, &ProjectWorkspace::whenDocumentOpen);
    disconnect(m_project->documentModel(), &ProjectDocumentModel::aboutToClose, this, &ProjectWorkspace::whenDocumentClose);

    for ( auto it = m_panes.begin(); it != m_panes.end(); ++it ){
        QQuickItem* pane = *it;
        disconnect(pane, &QQuickItem::widthChanged, this, &ProjectWorkspace::whenPaneSizeChanged);
        disconnect(pane, &QQuickItem::heightChanged, this, &ProjectWorkspace::whenPaneSizeChanged);
    }
    m_panes.clear();

    for ( auto it = m_watchers.begin(); it != m_watchers.end(); ++it ){
        delete *it;
    }
    m_watchers.clear();
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

void ProjectWorkspace::whenWindowInitialized(QQuickWindow *window){
    m_state->windows.append(window);

    connect(window, &QQuickWindow::xChanged, this, &ProjectWorkspace::whenWindowRectChanged);
    connect(window, &QQuickWindow::yChanged, this, &ProjectWorkspace::whenWindowRectChanged);
    connect(window, &QQuickWindow::widthChanged, this, &ProjectWorkspace::whenWindowRectChanged);
    connect(window, &QQuickWindow::heightChanged, this, &ProjectWorkspace::whenWindowRectChanged);
    connect(window, &QQuickWindow::visibilityChanged, this, &ProjectWorkspace::whenWindowVisibilityChanged);

    connect(window, SIGNAL(closing(QQuickCloseEvent*)), this, SLOT(whenWindowClose()));
}

void ProjectWorkspace::whenWindowOpen(QQuickWindow *window){
    connect(window, &QQuickWindow::xChanged, this, &ProjectWorkspace::whenWindowRectChanged);
    connect(window, &QQuickWindow::yChanged, this, &ProjectWorkspace::whenWindowRectChanged);
    connect(window, &QQuickWindow::widthChanged, this, &ProjectWorkspace::whenWindowRectChanged);
    connect(window, &QQuickWindow::heightChanged, this, &ProjectWorkspace::whenWindowRectChanged);
    connect(window, &QQuickWindow::visibilityChanged, this, &ProjectWorkspace::whenWindowVisibilityChanged);

    connect(window, SIGNAL(closing(QQuickCloseEvent*)), this, SLOT(whenWindowClose()));

    try{
        m_state->windowOpen(window);
    } catch ( lv::Exception& e ){
        lv::ViewContext::instance().engine()->throwError(&e, this);
    }
    emit windowOpen(window);
}

void ProjectWorkspace::whenWindowClose(){
    QQuickWindow* window = qobject_cast<QQuickWindow*>(sender());

    disconnect(window, &QQuickWindow::xChanged, this, &ProjectWorkspace::whenWindowRectChanged);
    disconnect(window, &QQuickWindow::yChanged, this, &ProjectWorkspace::whenWindowRectChanged);
    disconnect(window, &QQuickWindow::widthChanged, this, &ProjectWorkspace::whenWindowRectChanged);
    disconnect(window, &QQuickWindow::heightChanged, this, &ProjectWorkspace::whenWindowRectChanged);
    disconnect(window, &QQuickWindow::visibilityChanged, this, &ProjectWorkspace::whenWindowVisibilityChanged);

    try{
        m_state->windowClose(window);
    } catch ( lv::Exception& e ){
        lv::ViewContext::instance().engine()->throwError(&e, this);
    }
    emit windowClose(window);
}

void ProjectWorkspace::whenWindowRectChanged(){
    QQuickWindow* window = qobject_cast<QQuickWindow*>(sender());

    try{
        m_state->windowRectChanged(window, QRect(window->x(), window->y(), window->width(), window->height()));
    } catch ( lv::Exception& e ){
        lv::ViewContext::instance().engine()->throwError(&e, this);
    }
    emit windowRectChanged(window, QRect(window->x(), window->y(), window->width(), window->height()));
}

void ProjectWorkspace::whenWindowVisibilityChanged(){
    QQuickWindow* window = qobject_cast<QQuickWindow*>(sender());

    try{
        m_state->windowVisibilityChanged(window, window->visibility());
    } catch ( lv::Exception& e ){
        lv::ViewContext::instance().engine()->throwError(&e, this);
    }
    emit windowVisibilityChanged(window, window->visibility());
}

void ProjectWorkspace::whenPaneInitialized(QQuickItem *pane){
    m_panes.insert(pane);
    connect(pane, &QQuickItem::widthChanged, this, &ProjectWorkspace::whenPaneSizeChanged);
    connect(pane, &QQuickItem::heightChanged, this, &ProjectWorkspace::whenPaneSizeChanged);

    QmlPropertyWatcher* paneStateWatcher = new QmlPropertyWatcher(pane, "paneState");
    if ( paneStateWatcher->isValid() ){
        paneStateWatcher->onChange([this](const QmlPropertyWatcher& w){
            QQuickItem* p = static_cast<QQuickItem*>(w.object());
            QVariant v = w.read();
            try{
                m_state->paneStateChanged(p, v);
            } catch ( lv::Exception& e ){
                lv::ViewContext::instance().engine()->throwError(&e, this);
            }
            emit paneStateChanged(p, v);
        });
        m_watchers.append(paneStateWatcher);
    } else {
        delete paneStateWatcher;
    }

    QString paneType = pane->property("paneType").toString();
    int width = pane->property("width").toInt();
    int height = pane->property("height").toInt();

    vlog("appdata").v() << "Pane initialized: " << paneType << " " << width << "x" << height;
}

void ProjectWorkspace::whenPaneAdded(QQuickItem *pane, QQuickWindow *window, const QVariantList &position){
    m_panes.insert(pane);
    connect(pane, &QQuickItem::widthChanged, this, &ProjectWorkspace::whenPaneSizeChanged);
    connect(pane, &QQuickItem::heightChanged, this, &ProjectWorkspace::whenPaneSizeChanged);

    QmlPropertyWatcher* paneStateWatcher = new QmlPropertyWatcher(pane, "paneState");
    if ( paneStateWatcher->isValid() ){
        paneStateWatcher->onChange([this](const QmlPropertyWatcher& w){
            QQuickItem* p = static_cast<QQuickItem*>(w.object());
            QVariant v = w.read();
            try{
                m_state->paneStateChanged(p, v);
            } catch ( lv::Exception& e ){
                lv::ViewContext::instance().engine()->throwError(&e, this);
            }
            emit paneStateChanged(p, v);
        });
        m_watchers.append(paneStateWatcher);
    } else {
        delete paneStateWatcher;
    }

    m_state->paneAdded(pane, window, position);

    emit paneAdded(pane, window, position);
}

void ProjectWorkspace::whenPaneSizeChanged(){
    QQuickItem* pane = qobject_cast<QQuickItem*>(sender());
    try{
        m_state->paneSizeChanged(pane, QSize(pane->width(), pane->height()));
    } catch ( lv::Exception& e ){
        lv::ViewContext::instance().engine()->throwError(&e, this);
    }
    emit paneSizeChanged(pane, QSize(pane->width(), pane->height()));
}

void ProjectWorkspace::whenPaneRemoved(QQuickItem *pane){
    m_panes.remove(pane);
    disconnect(pane, &QQuickItem::widthChanged, this, &ProjectWorkspace::whenPaneSizeChanged);
    disconnect(pane, &QQuickItem::heightChanged, this, &ProjectWorkspace::whenPaneSizeChanged);
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
