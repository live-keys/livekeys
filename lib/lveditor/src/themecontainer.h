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

#ifndef LVTHEMECONTAINER_H
#define LVTHEMECONTAINER_H

#include <QObject>
#include <QAbstractListModel>
#include "live/lveditorglobal.h"

namespace lv{

class Theme;

/// \private
class ThemeContainer : public QAbstractListModel{

    Q_OBJECT
    Q_PROPERTY(lv::Theme* current READ current NOTIFY currentChanged)

public:
    enum Roles{
        Label = Qt::UserRole + 1
    };

public:
    explicit ThemeContainer(const QString& type = "", QObject *parent = 0);
    virtual ~ThemeContainer();

    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    QHash<int, QByteArray> roleNames() const;

    void addTheme(const QString& name, const QString& path);

    lv::Theme* current() const;

public slots:
    void setTheme(const QString& name);

signals:
    void currentChanged();

private:
    QMap<QString, QString>::const_iterator iteratorAt(int index) const;

    QString                m_type;
    QHash<int, QByteArray> m_roles;
    QMap<QString, QString> m_themes;
    lv::Theme*             m_current;
};

inline Theme *ThemeContainer::current() const{
    return m_current;
}

} // namespace

#endif // LVTHEMECONTAINER_H
