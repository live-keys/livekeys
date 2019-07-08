/****************************************************************************
**
** Copyright (C) 2014-2018 Dinu SV.
** (contact: mail@dinusv.com)
** This file is part of Live CV Application.
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
#include "live/lockedfileiosession.h"
#include "live/projectdocument.h"

#include <QObject>
#include <QHash>

class QTimer;

namespace lv{

class ViewEngine;
class ProjectEntry;
class ProjectFile;
class ProjectFileModel;
class ProjectNavigationModel;
class ProjectDocumentModel;

class LV_EDITOR_EXPORT Project : public QObject{

    Q_OBJECT
    Q_PROPERTY(lv::ProjectDocument*  active                READ active          NOTIFY activeChanged)
    Q_PROPERTY(lv::ProjectFileModel* fileModel             READ fileModel       CONSTANT)
    Q_PROPERTY(lv::ProjectNavigationModel* navigationModel READ navigationModel CONSTANT)
    Q_PROPERTY(lv::ProjectDocumentModel* documentModel     READ documentModel   CONSTANT)
    Q_PROPERTY(QString rootPath                            READ rootPath        NOTIFY pathChanged)
    Q_PROPERTY(lv::Project::RunTrigger runTrigger          READ runTrigger      WRITE setRunTrigger NOTIFY runTriggerChanged)
    Q_ENUMS(RunTrigger)

    friend class ProjectFileModel;
    friend class ProjectDocument;
    friend class ProjectDocumentModel;

public:
    enum RunTrigger{
        RunManual,
        RunOnSave,
        RunOnChange
    };

public:
    Project(QObject* parent = 0);
    ~Project();

    void setActive(const QString& rootPath);

    ProjectFile* lookupBestFocus(ProjectEntry* entry);
    ProjectDocument* isOpened(const QString& rootPath);

    /**
     * \brief Getter of the file model
     *
     * The file model represents all of the opened files in the current project
     */
    lv::ProjectFileModel* fileModel();

    /**
     * \brief Getter of the navigation model
     *
     * Used to search through the project for different files
     */
    lv::ProjectNavigationModel* navigationModel();
    /**
     * \brief Getter of the document model
     *
     * Shows all the opened files
     */
    lv::ProjectDocumentModel* documentModel();

    lv::ProjectDocument*  active() const;

    /**
     * \brief Getter of the root path
     *
     * Actual path of the project, whether it's a file- or folder-based project.
     */
    const QString& rootPath() const;

    LockedFileIOSession::Ptr lockedFileIO();

    lv::Project::RunTrigger runTrigger() const;
    void setRunTrigger(lv::Project::RunTrigger runTrigger);

    void setRunSpace(QObject* runSpace);

    static QByteArray hashPath(const QByteArray& path);

public slots:
    void newProject();
    void closeProject();
    lv::ProjectDocument* openFile(const QUrl& rootPath, int mode = lv::ProjectDocument::EditIfNotOpen);
    lv::ProjectDocument* openFile(const QString& rootPath, int mode = lv::ProjectDocument::EditIfNotOpen);
    lv::ProjectDocument* openFile(lv::ProjectFile* file, int mode = lv::ProjectDocument::EditIfNotOpen);
    void setActive(lv::ProjectFile *file);

    bool isDirProject() const;
    bool isFileInProject(const QUrl& rootPath) const;
    bool isFileInProject(const QString& rootPath) const;

    void openProject(const QString& rootPath);
    void openProject(const QUrl& url);

    void closeFile(const QString& rootPath);

    QString dir() const;
    QString path(const QString& relative) const;

    void scheduleRun();
    void run();

    QObject* runSpace();
    QObject* appRoot();

    void engineObjectAcquired(const QUrl& file);
    void engineObjectReady(QObject* object, const QUrl& file);

signals:
    /** path changed, means the whole project changed */
    void pathChanged(QString rootPath);
    /** active file has changed */
    void activeChanged(lv::ProjectDocument* active);

    /** triggers when a document is opened */
    void documentOpened(lv::ProjectDocument* document);

    /** refers to an internal project directory change, for example renaming */
    void directoryChanged(const QString& rootPath);
    /** file changed (e.g. on save) */
    void fileChanged(const QString& rootPath);

    /** run trigger changed */
    void runTriggerChanged();

    /** project is about to close */
    void aboutToClose();

private:
    ViewEngine* engine();
    ProjectFile* relocateDocument(const QString& rootPath, const QString &newPath, ProjectDocument *document);
    void setActive(ProjectDocument* document);
    void emptyRunSpace();
    ProjectDocument* createDocument(ProjectFile* file, bool isMonitored);
    void documentSaved(ProjectDocument* documnet);

private:
    ProjectFileModel*       m_fileModel;
    ProjectNavigationModel* m_navigationModel;
    ProjectDocumentModel*   m_documentModel;

    LockedFileIOSession::Ptr m_lockedFileIO;

    ProjectDocument* m_active;
    QString          m_path;
    QTimer*          m_scheduleRunTimer;
    RunTrigger       m_runTrigger;
    QObject*         m_runSpace;
    QObject*         m_appRoot;
};


inline ProjectFileModel* Project::fileModel(){
    return m_fileModel;
}

inline ProjectNavigationModel *Project::navigationModel(){
    return m_navigationModel;
}

inline ProjectDocumentModel *Project::documentModel(){
    return m_documentModel;
}

inline ProjectDocument *Project::active() const{
    return m_active;
}

inline const QString &Project::rootPath() const{
    return m_path;
}

/**
 * @brief Getter of the locked file session
 *
 * Implemention of the standard read-write model where there can be multiple readers, but a single writer,
 * and the write mode is locked for access.
 */
inline LockedFileIOSession::Ptr Project::lockedFileIO(){
    return m_lockedFileIO;
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


}// namespace

#endif // LVPROJECT_H
