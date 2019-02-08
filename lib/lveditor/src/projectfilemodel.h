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

#ifndef LVPROJECTFILEMODEL_H
#define LVPROJECTFILEMODEL_H

#include "live/lveditorglobal.h"

#include <QAbstractItemModel>
#include <functional>

namespace lv{

class ProjectEntry;
class ProjectFile;

class ProjectFileModel : public QAbstractItemModel{

    Q_OBJECT

public:
    enum Roles{
        UrlStringRole = Qt::UserRole + 1
    };
    Q_ENUM(Roles)

public:
    ProjectFileModel(QObject* parent = 0);
    ~ProjectFileModel();

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;

    QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const;
    QModelIndex rootIndex();
    QModelIndex parent(const QModelIndex &index) const;

    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;
    
    bool hasChildren(const QModelIndex &parent) const;

    Qt::ItemFlags flags(const QModelIndex &index) const;

    QHash<int, QByteArray> roleNames() const;

    void createProject();
    void openProject(const QString& path);
    void closeProject();

    ProjectFile* openFile(const QString& file);
    ProjectEntry* findPathInEntry(ProjectEntry* entry, const QString& path);
    ProjectEntry* root();

public slots:
    void entryRemoved(const QModelIndex& item);
    void entryRemoved(ProjectEntry* entry);
    void entryRemoved(const QModelIndex &item, lv::ProjectEntry* entry);
    void entryAdded(lv::ProjectEntry* item, lv::ProjectEntry* parent);

    void moveEntry(lv::ProjectEntry* item, lv::ProjectEntry* parent);
    void renameEntry(lv::ProjectEntry* item, const QString& newName);
    lv::ProjectFile* addFile(lv::ProjectEntry* parentEntry, const QString& name);
    lv::ProjectEntry* addDirectory(lv::ProjectEntry* parentEntry, const QString& name);
    bool removeEntry(lv::ProjectEntry* entry);

    void expandEntry(lv::ProjectEntry* entry) const;
    void rescanEntries(lv::ProjectEntry* entry = 0);

    lv::ProjectEntry *itemAt(const QModelIndex& index) const;

    QModelIndex itemIndex(lv::ProjectEntry* entry);

signals:
    void projectNodeChanged(QModelIndex index);
    void error(const QString& message);

private:
    ProjectFile* openExternalFile(const QString& file);
    ProjectEntry *itemOrRoot(const QModelIndex &index) const;

    ProjectEntry *m_root;
};

inline ProjectEntry *ProjectFileModel::root(){
    return m_root;
}

}// namespace

#endif // LVPROJECTFILEMODEL_H
