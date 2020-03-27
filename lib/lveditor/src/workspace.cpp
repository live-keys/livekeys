#include "workspace.h"
#include "workspacelayer.h"
#include "projectworkspace.h"
#include "projectdocumentmodel.h"

#include <QDir>
#include <QQuickItem>
#include <QQmlEngine>
#include <QQmlContext>
#include <QQmlProperty>

#include "live/viewcontext.h"
#include "live/viewengine.h"
#include "live/applicationcontext.h"
#include "live/windowlayer.h"
#include "live/visuallogqt.h"
#include "live/mlnode.h"
#include "live/mlnodetojson.h"

namespace lv{

// class Workspace::Message
// ------------------------

Workspace::Message::Message(Workspace::Message::Type type, const MLNode &data)
    : m_type(type)
    , m_data(data)
{
}

// class Workspace
// ---------------

Workspace::Workspace(Project *project, WorkspaceLayer *parent)
    : QObject(parent)
    , m_project(project)
    , m_currentProjectWorkspace(nullptr)
{
    connect(m_project, &Project::pathChanged, this, &Workspace::whenProjectPathChange);

    QString recentsPath = absolutePath("workspaces.json");
    QFile recentsFile(recentsPath);
    if ( recentsFile.exists() && recentsFile.open(QIODevice::ReadOnly) ){

        try{
            vlog("appdata").v() << "Loading recent projects from file: " << recentsPath;

            QByteArray result = recentsFile.readAll();
            MLNode recentsNode;
            ml::fromJson(result.constData(), recentsNode);

            MLNode projectsNode = recentsNode["projects"];

            for ( auto it = projectsNode.asArray().begin(); it != projectsNode.asArray().end(); ++it ){
                m_recents.append(QString::fromStdString(it->asString()));
            }

            recentsFile.close();

        } catch ( Exception& e ){
            vlog().w() << "Failed to parse file \'" << recentsPath << "\': " + e.message();
        }
    }
}

Workspace::~Workspace(){
    delete m_currentProjectWorkspace;
    saveRecents();
}

void Workspace::whenProjectPathChange(const QString &path){
    WorkspaceLayer* wl = qobject_cast<WorkspaceLayer*>(parent());
    if ( m_project->active() == nullptr && path == "" ){
        emit projectClose(m_currentProjectWorkspace);
        ProjectWorkspace* pw = m_currentProjectWorkspace;
        m_currentProjectWorkspace = nullptr;
        wl->whenProjectClose();
        delete pw;
    } else {
        emit projectClose(m_currentProjectWorkspace);
        ProjectWorkspace* pwold = m_currentProjectWorkspace;
        m_currentProjectWorkspace = nullptr;
        wl->whenProjectClose();
        delete pwold;

        if ( m_recents.isEmpty() || m_recents.front() != path ){
            for ( auto it = m_recents.begin(); it != m_recents.end(); ++it ){
                if ( *it == path ){
                    m_recents.erase(it);
                    break;
                }
            }
            m_recents.prepend(path);
            m_recentsChanged = true;
        }

        ProjectWorkspace* pw = ProjectWorkspace::create(m_project);

        emit projectOpen(path);

        m_currentProjectWorkspace = pw;
        wl->whenProjectOpen(path, pw);

        emit projectInitialized(path, m_currentProjectWorkspace);
    }
}

void Workspace::saveRecents(){
    QString recentsPath = absolutePath("workspaces.json");
    QFile recentsFile(recentsPath);
    if ( m_recentsChanged && recentsFile.open(QIODevice::WriteOnly) ){

        MLNode recents(MLNode::Array);

        for ( auto it = m_recents.begin(); it != m_recents.end(); ++it ){
            recents.append(MLNode(it->toStdString()));
        }

        MLNode n(MLNode::Object);
        n["projects"] = recents;

        std::string result;
        ml::toJson(n, result);

        vlog("appdata").v() << "Saving projects to: " << recentsPath;

        recentsFile.write(result.c_str());
        recentsFile.close();
    }
}


QString Workspace::absolutePath(const QString &appDataPath){
    return QString::fromStdString(lv::ApplicationContext::instance().appDataPath()) + "/" + appDataPath;
}

QString Workspace::absoluteDir(const QString &dir){
    QString d = absolutePath(dir);
    QDir ddir(d);
    if ( !ddir.exists() ){
        QDir().mkdir(d);
    }
    return d;
}

Workspace *Workspace::getFromContext(QQmlContext *ctx){
    QObject* lk = ctx->contextProperty("lk").value<QObject*>();
    if ( !lk ){
        return nullptr;
    }

    QObject* workspaceLayerOb = lk->property("layers").value<QQmlPropertyMap*>()->property("workspace").value<QObject*>();
    WorkspaceLayer* wlayer = static_cast<WorkspaceLayer*>(workspaceLayerOb);
    if ( !wlayer )
        return nullptr;

    return wlayer->workspace();
}

}// namespace
