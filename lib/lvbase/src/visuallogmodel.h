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

#ifndef LVVISUALLOGMODEL_H
#define LVVISUALLOGMODEL_H

#include <QDebug>

#include <QString>
#include <QAbstractListModel>
#include "live/lvbaseglobal.h"

class QQmlEngine;
class QQmlComponent;
class QQmlContext;

//TODO: Add log model filtering
//TODO: Manage caching for components

namespace lv{

class LV_BASE_EXPORT VisualLogEntry{
public:
    VisualLogEntry(const QString& tag, const QString& prefix, const QString& data);
    VisualLogEntry(const QString& tag, const QString& prefix, QVariant* objectData, QQmlComponent* component);

    QString                prefix;
    QString                tag;
    QString                data;
    QVariant*              objectData;
    mutable QQmlComponent* component;
    mutable QQmlContext*   context;
};


class LV_BASE_EXPORT VisualLogModel : public QAbstractListModel{

    Q_OBJECT
    Q_PROPERTY(int width READ width WRITE setWidth NOTIFY widthChanged)

public:
    VisualLogModel(QQmlEngine* engine);
    ~VisualLogModel();

    QVariant data(const QModelIndex &index, int role) const;
    int rowCount(const QModelIndex &parent) const;
    QHash<int, QByteArray> roleNames() const;

    const VisualLogEntry& entryAt(int index);

    void appendMessage(const QString& tag, const QString& prefix, const QString &message);
    void appendView(const QString& tag, const QString& prefix, const QString& viewName, const QVariant& value);

    int width() const;

public slots:
    void setWidth(int width);

signals:
    void widthChanged(int width);
    void entryAdded();

private:
    QQmlComponent* component(const QString& key);
    QString componentPath(const QString& componentKey);

    QQmlEngine*                    m_engine;
    QList<VisualLogEntry>          m_entries;
    QQmlComponent*                 m_textComponent;
    QHash<QString, QQmlComponent*> m_components;
    int                            m_width;
};

inline int VisualLogModel::rowCount(const QModelIndex &) const{
    return m_entries.size();
}

inline int VisualLogModel::width() const{
    return m_width;
}

inline void VisualLogModel::setWidth(int width){
    if (m_width == width)
        return;

    m_width = width;
    emit widthChanged(width);
}

}// namespace

#endif // VISUALLOGMODEL_H
