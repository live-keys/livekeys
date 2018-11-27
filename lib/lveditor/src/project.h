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
    void pathChanged(QString rootPath);
    void activeChanged(lv::ProjectDocument* active);

    void fileModelChanged(lv::ProjectFileModel* fileModel);
    void navigationModelChanged(lv::ProjectNavigationModel* navigationModel);
    void documentModelChanged(lv::ProjectDocumentModel* documentModel);

    void directoryChanged(const QString& rootPath);
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

inline LockedFileIOSession::Ptr Project::lockedFileIO(){
    return m_lockedFileIO;
}


}// namespace

#endif // LVPROJECT_H
