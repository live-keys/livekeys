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

#ifndef LVRUNNABLECONTAINER_H
#define LVRUNNABLECONTAINER_H

#include <QAbstractListModel>
#include <QQmlContext>

#include "live/lveditorglobal.h"

namespace lv{

class Project;
class Runnable;
class QmlBuild;

class LV_EDITOR_EXPORT RunnableContainer : public QAbstractListModel{

    Q_OBJECT

public:
    friend class Runnable;

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
    void qmlBuild(Runnable* runnable, QmlBuild* build);

public slots:
    void setFilter(const QString& filter);
    Runnable* runnableAt(const QString& path);

private:
    void announceQmlBuild(Runnable* runnable, QmlBuild* build);
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
