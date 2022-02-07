/****************************************************************************
**
** Copyright (C) 2022 Dinu SV.
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

#ifndef LVPROJECTFILEINDEXER_H
#define LVPROJECTFILEINDEXER_H

#include <QObject>
#include <QAbstractListModel>
#include <QHash>
#include <QFutureWatcher>
#include <QJSValue>

#include "live/lveditorglobal.h"

namespace lv{

class Project;
class ProjectDocument;
class ProjectDocumentModel;

/// \private
class ProjectFileIndexer : public QAbstractListModel{

    Q_OBJECT
    Q_PROPERTY(bool isIndexing READ isIndexing NOTIFY isIndexingChanged)

public:
    enum Roles{
        Name = Qt::UserRole + 1,
        Path,
        IsOpen
    };

    /// \private
    class Entry{
    public:
        Entry(const QString& pName, const QString& pPath) : name(pName), path(pPath){}

        QString name;
        QString path;
    };

public:
    ProjectFileIndexer(Project* project);
    ~ProjectFileIndexer();

    bool isIndexing() const;
    void beginIndexing();
    void endIndexing();

    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    QHash<int, QByteArray> roleNames() const;

    const QString& filter() const;

public slots:
    void filter(const QJSValue& options);
    void setFilter(const QString& filter);
    void reindex();
    void requiresReindex();

    void __reindexReady();
    void __directoryChanged(const QString& path);
signals:
    void isIndexingChanged(bool isIndexing);

private:
    void updateFilters();

    bool m_isIndexing;
    bool m_requiresReindex;
    QFutureWatcher<QList<Entry> > m_workerWatcher;

    Project*               m_project;
    ProjectDocumentModel*  m_projectDocuments;
    QList<Entry>           m_files;
    QList<Entry>           m_filteredOpenedFiles;
    QList<int>             m_filteredFiles;
    QHash<int, QByteArray> m_roles;
    QString                m_filter;
};

inline bool ProjectFileIndexer::isIndexing() const{
    return m_isIndexing;
}

inline void ProjectFileIndexer::beginIndexing(){
    m_isIndexing = true;
    emit isIndexingChanged(m_isIndexing);
}

inline void ProjectFileIndexer::endIndexing(){
    m_isIndexing = false;
    emit isIndexingChanged(m_isIndexing);
}

inline QHash<int, QByteArray> ProjectFileIndexer::roleNames() const{
    return m_roles;
}

inline const QString &ProjectFileIndexer::filter() const{
    return m_filter;
}

}// namespace

#endif // LVPROJECTFILEINDEXER_H
