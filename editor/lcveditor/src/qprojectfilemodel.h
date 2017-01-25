/****************************************************************************
**
** Copyright (C) 2014-2017 Dinu SV.
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

#ifndef QPROJECTFILEMODEL_H
#define QPROJECTFILEMODEL_H

#include "qlcveditorglobal.h"
#include <QAbstractItemModel>
#include <functional>

namespace lcv{

class QProjectEntry;
class QProjectFile;

class Q_LCVEDITOR_EXPORT QProjectFileModel : public QAbstractItemModel{

    Q_OBJECT

public:
    enum Roles{
        UrlStringRole = Qt::UserRole + 1
    };
    Q_ENUM(Roles)

public:
    QProjectFileModel(QObject* parent = 0);
    ~QProjectFileModel();

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

    QProjectFile* openFile(const QString& file);
    QProjectEntry* findPathInEntry(QProjectEntry* entry, const QString& path);
    QProjectEntry* root();

public slots:
    void entryRemoved(const QModelIndex& item);
    void entryRemoved(QProjectEntry* entry);
    void entryRemoved(const QModelIndex &item, lcv::QProjectEntry* entry);
    void entryAdded(lcv::QProjectEntry* item, lcv::QProjectEntry* parent);

    void moveEntry(lcv::QProjectEntry* item, lcv::QProjectEntry* parent);
    void renameEntry(lcv::QProjectEntry* item, const QString& newName);
    lcv::QProjectFile* addFile(lcv::QProjectEntry* parentEntry, const QString& name);
    lcv::QProjectEntry* addDirectory(lcv::QProjectEntry* parentEntry, const QString& name);
    bool removeEntry(lcv::QProjectEntry* entry);

    void expandEntry(lcv::QProjectEntry* entry) const;
    void rescanEntries(lcv::QProjectEntry* entry = 0);

    lcv::QProjectEntry *itemAt(const QModelIndex& index) const;

    QModelIndex itemIndex(lcv::QProjectEntry* entry);

signals:
    void projectNodeChanged(QModelIndex index);
    void error(const QString& message);

private:
    QProjectFile* openExternalFile(const QString& file);
    QProjectEntry *itemOrRoot(const QModelIndex &index) const;

    QProjectEntry *m_root;
};

inline QProjectEntry *QProjectFileModel::root(){
    return m_root;
}

}// namespace

#endif // QPROJECTFILEMODEL_H
