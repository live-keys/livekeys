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

#ifndef LVVISUALLOGBASEMODEL_H
#define LVVISUALLOGBASEMODEL_H

#include <QObject>
#include <QAbstractListModel>
#include "live/lvbaseglobal.h"


class QQmlComponent;
class QQmlContext;

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

class LV_BASE_EXPORT VisualLogBaseModel : public QAbstractListModel{

    Q_OBJECT

public:
    enum Roles{
        Msg = Qt::UserRole + 1,
        Prefix
    };

public:
    explicit VisualLogBaseModel(QObject *parent = nullptr);
    virtual ~VisualLogBaseModel();

    virtual QVariant data(const QModelIndex &index, int role) const = 0;
    virtual int rowCount(const QModelIndex &parent) const = 0;
    virtual QHash<int, QByteArray> roleNames() const;

    virtual int totalEntries() const = 0;
    virtual QVariant entryDataAt(int index) const = 0;
    virtual QString entryPrefixAt(int index) const = 0;
    virtual const VisualLogEntry &entryAt(int index) const = 0;

};

}// namespace

#endif // LVVISUALLOGBASEMODEL_H
