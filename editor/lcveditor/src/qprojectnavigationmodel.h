#ifndef QPROJECTNAVIGATIONMODEL_H
#define QPROJECTNAVIGATIONMODEL_H

#include <QObject>
#include <QAbstractListModel>
#include <QHash>
#include <QFutureWatcher>
#include "qlcveditorglobal.h"

namespace lcv{

class QProject;
class QProjectDocument;
class Q_LCVEDITOR_EXPORT QProjectNavigationModel : public QAbstractListModel{

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
    QProjectNavigationModel(QProject* project);
    ~QProjectNavigationModel();

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

    QList<Entry>           m_files;
    QList<Entry>           m_filteredOpenedFiles;
    QList<int>             m_filteredFiles;
    QHash<int, QByteArray> m_roles;
    QString                m_filter;
    QProject*              m_project;
};

inline bool QProjectNavigationModel::isIndexing() const{
    return m_isIndexing;
}

inline void QProjectNavigationModel::beginIndexing(){
    m_isIndexing = true;
    emit isIndexingChanged(m_isIndexing);
}

inline void QProjectNavigationModel::endIndexing(){
    m_isIndexing = false;
    emit isIndexingChanged(m_isIndexing);
}

inline QHash<int, QByteArray> QProjectNavigationModel::roleNames() const{
    return m_roles;
}

inline const QString &QProjectNavigationModel::filter() const{
    return m_filter;
}

}// namespace

#endif // QPROJECTNAVIGATIONMODEL_H
