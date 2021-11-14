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

#ifndef LVPROJECT_H
#define LVPROJECT_H

#include "live/lveditorglobal.h"
#include "live/projectdocument.h"
#include "live/document.h"

#include "live/runnable.h"

#include <QObject>
#include <QHash>

class QTimer;

namespace lv{

class Runnable;
class ViewEngine;
class ProjectEntry;
class ProjectFile;
class ProjectFileModel;
class ProjectNavigationModel;
class ProjectDocumentModel;
class RunnableContainer;
class ProjectEngineInterceptor;
class Program;
class ProgramHolder;
class WorkspaceLayer;

class LV_EDITOR_EXPORT Project : public QObject{

    Q_OBJECT
    Q_PROPERTY(lv::Runnable* active                        READ active          NOTIFY activeChanged)
    Q_PROPERTY(lv::ProjectFileModel* fileModel             READ fileModel       CONSTANT)
    Q_PROPERTY(lv::ProjectNavigationModel* navigationModel READ navigationModel CONSTANT)
    Q_PROPERTY(lv::ProjectDocumentModel* documentModel     READ documentModel   CONSTANT)
    Q_PROPERTY(lv::RunnableContainer* runnables            READ runnables       CONSTANT)
    Q_PROPERTY(QString rootPath                            READ rootPath        NOTIFY pathChanged)
    Q_PROPERTY(lv::Project::RunTrigger runTrigger          READ runTrigger      WRITE setRunTrigger NOTIFY runTriggerChanged)
    Q_ENUMS(RunTrigger)

    friend class ProjectFileModel;
    friend class Document;
    friend class ProjectDocument;
    friend class ProjectDocumentModel;
    friend class Runnable;

public:
    enum RunTrigger{
        RunManual,
        RunOnSave,
        RunOnChange
    };

public:
    Project(WorkspaceLayer* workspaceLayer, QObject* parent = nullptr);
    ~Project();

    ProjectFile* lookupBestFocus(ProjectEntry* entry);
    Document* isOpened(const QString& rootPath);

    lv::ProjectFileModel* fileModel();
    lv::ProjectNavigationModel* navigationModel();
    lv::ProjectDocumentModel* documentModel();
    lv::RunnableContainer* runnables() const;

    lv::Runnable* active() const;

    /**
     * \brief Getter of the root path
     *
     * Actual path of the project, whether it's a file- or folder-based project.
     */
    const QString& rootPath() const;

    lv::Project::RunTrigger runTrigger() const;
    void setRunTrigger(lv::Project::RunTrigger runTrigger);

    void setRunSpace(QObject* runSpace);

    static QByteArray hashPath(const QByteArray& path);

    void excludeRunTriggers(const QSet<QString>& paths);
    void removeExcludedRunTriggers(const QSet<QString>& paths);

    void monitorFiles(const QStringList& files);

    el::Engine* engine();

    Document *openFile(
        const QString& path,
        const QString& type,
        int mode,
        const QString& format = ""
    );

public slots:
    void newProject();
    void closeProject();
    void openProject(const QUrl& path);

    lv::Document* isOpened(const QUrl& url);
    lv::Document* openFile(const QUrl& url, const QJSValue& options);
    bool isFileInProject(const QUrl& url);
    void closeFile(const QString& rootPath);
    void setActive(const QString& rootPath);
    lv::Document* createDocument(const QJSValue& options);

    lv::Runnable* openRunnable(const QString& path, const QStringList& activations = QStringList());

    bool isDirProject() const;
    bool canRunFile(const QString& path) const;

    QString dir() const;
    QString path(const QString& relative) const;

    void scheduleRun();
    void run();

    QObject* runSpace();
    QObject* appRoot();

signals:
    /** path changed, means the whole project changed */
    void pathChanged(QString rootPath);
    /** active file has changed */
    void activeChanged(lv::Runnable* active);

    /** triggers when a document is opened */
    void documentOpened(lv::Document* document);

    /** refers to an internal project directory change, for example renaming */
    void directoryChanged(const QString& rootPath);
    /** file changed (e.g. on save) */
    void fileChanged(const QString& rootPath);

    /** run trigger changed */
    void runTriggerChanged();

    /** project is about to close */
    void aboutToClose();

private:
    void openProject(const QUrl& url, Program* main);
    QString findFileFormat(const QString& path);
    QString localPath(const QUrl& url);
    ViewEngine* viewEngine();
    ProjectFile* relocateDocument(const QString& rootPath, const QString &newPath, Document *document);
    void setActive(Runnable* runnable);
    ProjectDocument* createTextDocument(ProjectFile* file, const QString &format, bool isMonitored);
    Document* createDocument(ProjectFile* file, const QString& formatType, bool isMonitored);
    void documentSaved(Document *documnet);

private:
    WorkspaceLayer*           m_workspaceLayer;
    ProjectFileModel*         m_fileModel;
    ProjectNavigationModel*   m_navigationModel;
    ProjectDocumentModel*     m_documentModel;
    RunnableContainer*        m_runnables;
    ProgramHolder*            m_programHolder;

    Runnable*        m_active;
    QString          m_path;
    QObject*         m_runspace;
    QTimer*          m_scheduleRunTimer;
    RunTrigger       m_runTrigger;

    QSet<QString>    m_excludedRunTriggers;
    el::Engine*      m_engine;
};


/**
 * \brief Getter of the file model
 *
 * The file model represents all of the opened files in the current project
 */
inline ProjectFileModel* Project::fileModel(){
    return m_fileModel;
}

/**
 * \brief Getter of the navigation model
 *
 * Used to search through the project for different files
 */
inline ProjectNavigationModel *Project::navigationModel(){
    return m_navigationModel;
}
/**
 * \brief Getter of the document model
 *
 * Shows all the opened files
 */

inline ProjectDocumentModel *Project::documentModel(){
    return m_documentModel;
}

/**
 * \brief Returns the opened runnables
 */
inline RunnableContainer *Project::runnables() const{
    return m_runnables;
}

inline Runnable* Project::active() const{
    return m_active;
}

inline const QString &Project::rootPath() const{
    return m_path;
}

/**
 * \brief Returns the trigger type to rerun the project.
 */
inline Project::RunTrigger Project::runTrigger() const{
    return m_runTrigger;
}


/**
 * \brief Sets the trigger type to rerun the project.
 */
inline void Project::setRunTrigger(Project::RunTrigger runTrigger){
    if (m_runTrigger == runTrigger)
        return;

    m_runTrigger = runTrigger;
    emit runTriggerChanged();
}

/**
 * \brief Returns the Elements engine associated with this project.
 */
inline el::Engine *Project::engine(){
    return m_engine;
}


}// namespace

#endif // LVPROJECT_H
