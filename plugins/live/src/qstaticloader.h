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

#ifndef QSTATICLOADER_H
#define QSTATICLOADER_H

#include "qliveglobal.h"
#include <QObject>
#include <QQmlComponent>
#include <QQuickItem>

class QStaticLoaderItem;

class QStaticLoader : public QQuickItem{

    Q_OBJECT
    Q_PROPERTY(QQmlComponent* source READ source  WRITE setSource  NOTIFY sourceChanged)
    Q_PROPERTY(QObject*       item   READ item    NOTIFY itemChanged)

public:
    explicit QStaticLoader(QQuickItem *parent = 0);
    ~QStaticLoader();

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
    QStaticLoaderItem* m_data;

};

inline QQmlComponent *QStaticLoader::source() const{
    return m_source;
}

#endif // QSTATICLOADER_H
