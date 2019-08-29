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

#ifndef LVPROJECTDOCUMENTMODEL_H
#define LVPROJECTDOCUMENTMODEL_H

#include <QAbstractListModel>

#include "live/lveditorglobal.h"

class QFileSystemWatcher;

namespace lv{

class Project;
class ProjectDocument;
class LV_EDITOR_EXPORT ProjectDocumentModel : public QAbstractListModel{

    Q_OBJECT

public:
    /** Model roles */
    enum Roles{
        Name = Qt::UserRole + 1,
        Path,
        IsOpen
    };

public:
    ProjectDocumentModel(Project* project);
    ~ProjectDocumentModel();

    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    QHash<int, QByteArray> roleNames() const;

    void openDocument(const QString& path, ProjectDocument* document);
    void relocateDocument(const QString& path, const QString& newPath, ProjectDocument* document);
    void closeDocuments();

    const QHash<QString, ProjectDocument*> openedFiles() const;

    void updateDocumentMonitoring(ProjectDocument* document, bool monitor);

    void closeDocumentsInPath(const QString& path);
    void closeDocument(const QString& path);

public slots:
    void rescanDocuments();
    void monitoredFileChanged(const QString& path);

    bool saveDocuments();
    lv::ProjectDocument* isOpened(const QString& path);
    lv::ProjectDocument* lastOpened();

    lv::ProjectDocument* documentByPathHash(const QString& pathHash);

    QStringList listUnsavedDocuments();
    QStringList listUnsavedDocumentsInPath(const QString& path);

signals:
    /** Shows that the given document has changed */
    void monitoredDocumentChanged(lv::ProjectDocument* document);
    /** Shows that the given document was changed externally */
    void documentChangedOutside(lv::ProjectDocument* document);
    /** Signals that the document is being closed */
    void aboutToClose(lv::ProjectDocument* document);

private:
    QHash<int, QByteArray> m_roles;

    QHash<QString, ProjectDocument*> m_openedFiles;

    QFileSystemWatcher* fileWatcher();
    QFileSystemWatcher* m_fileWatcher;
};

/** Returns the hashmap containing the open documents */
inline const QHash<QString, ProjectDocument *> ProjectDocumentModel::openedFiles() const{
    return m_openedFiles;
}

}// namespace

#endif // LVPROJECTDOCUMENTMODEL_H
