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

#ifndef LVSTATICLOADER_H
#define LVSTATICLOADER_H

#include "live/lvliveglobal.h"
#include <QObject>
#include <QQmlComponent>
#include <QQuickItem>

namespace lv{

class StaticLoaderItem;

/// \private
class StaticLoader : public QQuickItem{

    Q_OBJECT
    Q_PROPERTY(QQmlComponent* source READ source  WRITE setSource  NOTIFY sourceChanged)
    Q_PROPERTY(QObject*       item   READ item    NOTIFY itemChanged)

public:
    explicit StaticLoader(QQuickItem *parent = 0);
    ~StaticLoader();

    QQmlComponent* source() const;
    QObject* item() const;

signals:
    void sourceChanged();
    void itemChanged();
    void itemCreated();

public slots:
    void setSource(QQmlComponent* arg);
    void staticLoad(const QString& id);

private:
    void createObject();

    QQmlComponent*     m_source;
    StaticLoaderItem* m_data;

};

inline QQmlComponent *StaticLoader::source() const{
    return m_source;
}

}// namespace

#endif // LVSTATICLOADER_H
