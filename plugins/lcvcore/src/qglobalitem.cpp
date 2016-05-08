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

#include "qglobalitem.h"
#include "qstatecontainer.h"
#include <QQmlEngine>

class QGlobalItemState{
public:
    QGlobalItemState() : item(0){}
    QQuickItem* item;

    ~QGlobalItemState(){
    }
};

QGlobalItem::QGlobalItem(QQuickItem* parent)
    : QQuickItem(parent)
    , m_source(0)
    , m_stateId("")
    , m_state(0)
{
}

QGlobalItem::~QGlobalItem(){
    if ( m_stateId == "" )
        delete m_state;
    else if ( m_state->item )
        m_state->item->setParentItem(0);
}

void QGlobalItem::setStateId(const QString& arg){
    if (m_stateId == arg)
        return;
    if (m_stateId == "")
        delete m_state;
    else
        m_state = 0;

    m_stateId = arg;
    emit stateIdChanged(arg);

    sync();
}

void QGlobalItem::sync(){
    if ( !isComponentComplete() || m_source == 0 )
        return;

    if ( !m_state ){
        if ( m_stateId != "" ){
            QStateContainer<QGlobalItemState>& stateContainer = QStateContainer<QGlobalItemState>::instance(this);
            m_state = stateContainer.state(m_stateId);
            if ( m_state == 0 ){
                m_state = new QGlobalItemState;
                reload();
                stateContainer.registerState(m_stateId, m_state);
            } else {
                m_state->item->setParentItem(parentItem());
                m_state->item->stackAfter(this);
            }

        } else {
            qCritical("GlobalItem will not function without a stateId assigned. "
                      "Assign a stateId key to fix this problem.");
            m_state = new QGlobalItemState;
            reload();
        }
    }
}

void QGlobalItem::componentComplete(){
    QQuickItem::componentComplete();
    sync();
}

QQuickItem* QGlobalItem::item() const{
    if (!m_state)
        return 0;
    return m_state->item;
}

void QGlobalItem::reload(){
    if ( !isComponentComplete() || m_source == 0 )
        return;

    QQuickItem* item = qobject_cast<QQuickItem*>(m_source->create(m_source->creationContext()));
    if ( item == 0 ){
        qCritical("GlobalItem: Failed to create item: %s", qPrintable(m_source->errorString()));
        return;
    }

    if ( m_state->item != 0 ){
        m_state->item->setParentItem(0);
        m_state->item->deleteLater();
        m_state->item = 0;
    }
    m_state->item = item;
    m_state->item->setParentItem(parentItem());
    m_state->item->stackAfter(this);
    qmlEngine(this)->setObjectOwnership(m_state->item, QQmlEngine::CppOwnership);

    emit itemChanged();
}
