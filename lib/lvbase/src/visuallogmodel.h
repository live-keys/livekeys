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

#include <QString>
#include <QAbstractListModel>
#include "live/visuallog.h"
#include "live/visuallogbasemodel.h"
#include "live/lvbaseglobal.h"

class QQmlEngine;

//TODO: Manage caching for components

namespace lv{

class LV_BASE_EXPORT VisualLogModel : public VisualLogBaseModel{

    Q_OBJECT
    Q_PROPERTY(int width READ width WRITE setWidth NOTIFY widthChanged)

public:
    VisualLogModel(QQmlEngine* engine);
    ~VisualLogModel();

    QVariant data(const QModelIndex &index, int role) const;
    int rowCount(const QModelIndex &parent) const;

    const VisualLogEntry& entryAt(int index);

    void onMessage(
        const VisualLog::Configuration* configuration,
        const VisualLog::MessageInfo& messageInfo,
        const QString& message
    );
    void onView(
        const VisualLog::Configuration* configuration,
        const VisualLog::MessageInfo& messageInfo,
        const QString& viewName,
        const QVariant& value
    );

    int width() const;

    int totalEntries() const;
    QVariant entryDataAt(int index) const;
    QString entryPrefixAt(int index) const;
    const VisualLogEntry &entryAt(int index) const;

    QList<VisualLogEntry>::Iterator begin();
    QList<VisualLogEntry>::Iterator end();
    QList<VisualLogEntry>::ConstIterator begin() const;
    QList<VisualLogEntry>::ConstIterator end() const;

public slots:
    void setWidth(int width);
    void clearValues();

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

inline int VisualLogModel::totalEntries() const{
    return m_entries.size();
}

inline QList<VisualLogEntry>::Iterator VisualLogModel::begin(){
    return m_entries.begin();
}

inline QList<VisualLogEntry>::Iterator VisualLogModel::end(){
    return m_entries.end();
}

inline QList<VisualLogEntry>::ConstIterator VisualLogModel::begin() const{
    return m_entries.begin();
}

inline QList<VisualLogEntry>::ConstIterator VisualLogModel::end() const{
    return m_entries.end();
}

}// namespace

#endif // VISUALLOGMODEL_H
