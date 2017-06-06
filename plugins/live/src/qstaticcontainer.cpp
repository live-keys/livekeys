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

#include "qstaticcontainer.h"
#include <QQuickWindow>
#include <QQuickItem>
#include <QQmlContext>

QStaticContainer::QStaticContainer(QObject *parent)
    : QObject(parent)
{
}

QStaticContainer::~QStaticContainer(){
    for ( QLinkedList<QStaticTypeContainerBase*>::iterator it = m_stateContainerList.begin(); it != m_stateContainerList.end(); ++it )
        delete *it;
    m_stateContainerList.clear();
}

void QStaticContainer::statecontainer(QStaticTypeContainerBase *container){
    m_stateContainerList.append(container);
}

QStaticContainer *QStaticContainer::grabFromContext(QQuickItem *item, const QString &contextProperty){
    return static_cast<QStaticContainer*>(qmlContext(item)->contextProperty(contextProperty).value<QObject*>());
}

void QStaticContainer::beforeCompile(){
    QSTATIC_ITEM_CONTAINER_DEBUG("-----Before Compile-----");
    for ( QLinkedList<QStaticTypeContainerBase*>::iterator it = m_stateContainerList.begin(); it != m_stateContainerList.end(); ++it )
        (*it)->beforeCompile();
}

void QStaticContainer::afterCompile(){
    QSTATIC_ITEM_CONTAINER_DEBUG("-----After Compile-----");
    for ( QLinkedList<QStaticTypeContainerBase*>::iterator it = m_stateContainerList.begin(); it != m_stateContainerList.end(); ++it )
        (*it)->afterCompile();
}

void QStaticContainer::clearStates(){
    QSTATIC_ITEM_CONTAINER_DEBUG("-----Clear States-----");
    for ( QLinkedList<QStaticTypeContainerBase*>::iterator it = m_stateContainerList.begin(); it != m_stateContainerList.end(); ++it )
        (*it)->clearStates();
}
