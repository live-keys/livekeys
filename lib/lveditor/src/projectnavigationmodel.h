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

#ifndef LVPROJECTNAVIGATIONMODEL_H
#define LVPROJECTNAVIGATIONMODEL_H

#include <QObject>
#include <QAbstractListModel>
#include <QHash>
#include <QFutureWatcher>

#include "live/lveditorglobal.h"

namespace lv{

class Project;
class ProjectDocument;
class LV_EDITOR_EXPORT ProjectNavigationModel : public QAbstractListModel{

    Q_OBJECT
    Q_PROPERTY(bool isIndexing READ isIndexing NOTIFY isIndexingChanged)

public:
    enum Roles{
        Name = Qt::UserRole + 1,
        Path,
        IsOpen
    };

    class Entry{
    public:
        Entry(const QString& pName, const QString& pPath) : name(pName), path(pPath){}

        QString name;
        QString path;
    };

public:
    ProjectNavigationModel(Project* project);
    ~ProjectNavigationModel();

    bool isIndexing() const;
    void beginIndexing();
    void endIndexing();

    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    QHash<int, QByteArray> roleNames() const;

    const QString& filter() const;

public slots:
    void setFilter(const QString& filter);
    void directoryChanged(const QString& path);
    void reindex();
    void reindexReady();
    void requiresReindex();

signals:
    void isIndexingChanged(bool isIndexing);

private:
    void updateFilters();

    bool m_isIndexing;
    bool m_requiresReindex;
    QFutureWatcher<QList<Entry> > m_workerWatcher;

    Project*               m_project;
    QList<Entry>           m_files;
    QList<Entry>           m_filteredOpenedFiles;
    QList<int>             m_filteredFiles;
    QHash<int, QByteArray> m_roles;
    QString                m_filter;
};

inline bool ProjectNavigationModel::isIndexing() const{
    return m_isIndexing;
}

inline void ProjectNavigationModel::beginIndexing(){
    m_isIndexing = true;
    emit isIndexingChanged(m_isIndexing);
}

inline void ProjectNavigationModel::endIndexing(){
    m_isIndexing = false;
    emit isIndexingChanged(m_isIndexing);
}

inline QHash<int, QByteArray> ProjectNavigationModel::roleNames() const{
    return m_roles;
}

inline const QString &ProjectNavigationModel::filter() const{
    return m_filter;
}

}// namespace

#endif // LVPROJECTNAVIGATIONMODEL_H
