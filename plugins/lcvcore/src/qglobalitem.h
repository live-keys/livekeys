/****************************************************************************
**
** Copyright (C) 2014-2016 Dinu SV.
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

#ifndef QGLOBALITEM_H
#define QGLOBALITEM_H

#include <QQmlComponent>
#include <QQuickItem>

class QGlobalItemState;
class QGlobalItem : public QQuickItem{

    Q_OBJECT
    Q_PROPERTY(QQmlComponent* source READ source  WRITE setSource  NOTIFY sourceChanged)
    Q_PROPERTY(QString     stateId   READ stateId WRITE setStateId NOTIFY stateIdChanged)
    Q_PROPERTY(QQuickItem* item      READ item    NOTIFY itemChanged)

public:
    explicit QGlobalItem(QQuickItem *parent = 0);
    ~QGlobalItem();

    QQmlComponent* source() const;
    QQuickItem* item() const;

    const QString& stateId() const;

signals:
    void sourceChanged();
    void itemChanged();

    void stateIdChanged(QString arg);

public slots:
    void setSource(QQmlComponent* arg);
    void setStateId(const QString &arg);

    void reload();

protected:
    void componentComplete();

private:
    void sync();

    QQmlComponent*    m_source;
    QString           m_stateId;
    QGlobalItemState* m_state;
};

inline QQmlComponent *QGlobalItem::source() const{
    return m_source;
}

inline void QGlobalItem::setSource(QQmlComponent *arg){
    if (m_source == arg)
        return;

    m_source = arg;
    emit sourceChanged();

    sync();
    reload();
}

inline const QString& QGlobalItem::stateId() const{
    return m_stateId;
}

#endif // QGLOBALITEM_H
