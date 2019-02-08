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

namespace lv{

class ProjectEntry;
class ProjectFile;
class ProjectFileModel;
class ProjectNavigationModel;
class ProjectDocumentModel;

class LV_EDITOR_EXPORT Project : public QObject{

    Q_OBJECT
    Q_PROPERTY(lv::ProjectDocument*  active                READ active          NOTIFY activeChanged)
    Q_PROPERTY(lv::ProjectFileModel* fileModel             READ fileModel       NOTIFY fileModelChanged)
    Q_PROPERTY(lv::ProjectNavigationModel* navigationModel READ navigationModel NOTIFY navigationModelChanged)
    Q_PROPERTY(lv::ProjectDocumentModel* documentModel     READ documentModel   NOTIFY documentModelChanged)
    Q_PROPERTY(QString rootPath                            READ rootPath        NOTIFY pathChanged)

    friend class ProjectFileModel;
    friend class ProjectDocument;
    friend class ProjectDocumentModel;

public:
    Project(QObject* parent = 0);
    ~Project();

    void setActive(const QString& rootPath);

    ProjectFile* lookupBestFocus(ProjectEntry* entry);

    ProjectDocument* isOpened(const QString& rootPath);

    lv::ProjectFileModel* fileModel();
    lv::ProjectNavigationModel* navigationModel();
    lv::ProjectDocumentModel* documentModel();
    lv::ProjectDocument*  active() const;

    const QString& rootPath() const;

    LockedFileIOSession::Ptr lockedFileIO();

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

signals:
    /** path changed, means the whole project changed */
    void pathChanged(QString rootPath);
    /** active file has changed */
    void activeChanged(lv::ProjectDocument* active);

    /** file model changed */
    void fileModelChanged(lv::ProjectFileModel* fileModel);
    /** navigation model changed */
    void navigationModelChanged(lv::ProjectNavigationModel* navigationModel);
    /** document model changed */
    void documentModelChanged(lv::ProjectDocumentModel* documentModel);

    /** refers to an internal project directory change, for example renaming */
    void directoryChanged(const QString& rootPath);
    /** file changed (e.g. on save) */
    void fileChanged(const QString& rootPath);

private:
    ProjectFile* relocateDocument(const QString& rootPath, const QString &newPath, ProjectDocument *document);
    void setActive(ProjectDocument* document);

private:
    ProjectFileModel*       m_fileModel;
    ProjectNavigationModel* m_navigationModel;
    ProjectDocumentModel*   m_documentModel;

    LockedFileIOSession::Ptr m_lockedFileIO;

    ProjectDocument* m_active;
    QString          m_path;
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
 * \brief Getter of the currently active project document
 */
inline ProjectDocument *Project::active() const{
    return m_active;
}

/**
 * \brief Getter of the root path
 *
 * Actual path of the project, whether it's a file- or folder-based project.
 */
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


}// namespace

#endif // LVPROJECT_H
