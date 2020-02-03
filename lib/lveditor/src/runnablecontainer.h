#ifndef LVRUNNABLECONTAINER_H
#define LVRUNNABLECONTAINER_H

#include <QAbstractListModel>

#include "live/lveditorglobal.h"

namespace lv{

class Project;
class Runnable;

class LV_EDITOR_EXPORT RunnableContainer : public QAbstractListModel{

    Q_OBJECT

public:
    /** Model roles */
    enum Roles{
        Name = Qt::UserRole + 1,
        Path
    };

public:
    explicit RunnableContainer(Project *parent = nullptr);
    ~RunnableContainer();

    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    QHash<int, QByteArray> roleNames() const;

    void addRunnable(Runnable* runnable);
    void closeRunnable(Runnable* runnable);
    void clearAll();

    QList<Runnable*> runnablesInPath(const QString& path);
    Runnable* runnableAt(int index);

signals:

public slots:
    void setFilter(const QString& filter);
    Runnable* runnableAt(const QString& path);

private:
    void updateFilters();

    QHash<int, QByteArray> m_roles;
    QList<Runnable*>       m_runnables;

    QString                m_filter;
    QList<int>             m_filteredRunnables;
};

inline QHash<int, QByteArray> RunnableContainer::roleNames() const{
    return m_roles;
}

}// namespace

#endif // LVRUNNABLECONTAINER_H
