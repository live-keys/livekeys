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

#include "qstaticcontainer.h"
#include "live/visuallog.h"
#include "live/visuallogqt.h"
#include <QQuickWindow>
#include <QQuickItem>
#include <QQmlContext>


/*!
  \class QStaticContainer
  \brief Manages static items and assures their deletion when appropriate
  \inmodule live_cpp

  Check the section on \l {Creating a Static Item}{creating a static item} on how to use
  this component.
*/


/*!
  \brief QStaticContainer constructor

  Parameters:
  \a parent
 */
QStaticContainer::QStaticContainer(QObject *parent)
    : QObject(parent)
{
}

/*!
 \brief QStaticContainer destructor
 */
QStaticContainer::~QStaticContainer(){
    for ( QLinkedList<QStaticTypeContainerBase*>::iterator it = m_stateContainerList.begin(); it != m_stateContainerList.end(); ++it )
        delete *it;
    m_stateContainerList.clear();
}

/*!
  \brief Private method used internally to store the \a container to be cleaned up later.
 */
void QStaticContainer::statecontainer(QStaticTypeContainerBase *container){
    m_stateContainerList.append(container);
}

/*!
  \fn T *QStaticContainer::get(const QString &key)
  \brief Returns the value given at \a key or 0 if it doesnt exist.
*/

/*!
  \fn void QStaticContainer::set(const QString &key, T *value)
  \brief Stores the \a value at \a key.
*/

/*!
  \fn void QStaticContainer::beforeCompile()
  \brief Private slot used to receive beforeCompile signals from the engine
*/

/*!
  \fn void QStaticContainer::afterCompile()
  \brief Private slot used to receive afterCompile signals from the engine
*/

/*!
  \fn void QStaticContainer::clearStates()
  \brief Private slot used to receive targetChanged signals from the engine
*/

/*!
  \brief Returns the state container associated with the items context.

  Params:
  \a item
  \a contextProperty
 */
QStaticContainer *QStaticContainer::grabFromContext(QQuickItem *item, const QString &contextProperty){
    return static_cast<QStaticContainer*>(qmlContext(item)->contextProperty(contextProperty).value<QObject*>());
}

void QStaticContainer::debugMessage(const QString &message){
    vlog("live-staticcontainer").v() << message;
}

void QStaticContainer::beforeCompile(){
    vlog_debug("live-staticcontainer", "-----Before Compile-----");
    for ( QLinkedList<QStaticTypeContainerBase*>::iterator it = m_stateContainerList.begin(); it != m_stateContainerList.end(); ++it )
        (*it)->beforeCompile();
}

void QStaticContainer::afterCompile(){
    vlog_debug("live-staticcontainer", "-----After Compile-----");
    for ( QLinkedList<QStaticTypeContainerBase*>::iterator it = m_stateContainerList.begin(); it != m_stateContainerList.end(); ++it )
        (*it)->afterCompile();
}

void QStaticContainer::clearStates(){
    vlog_debug("live-staticcontainer", "-----Clear States-----");
    for ( QLinkedList<QStaticTypeContainerBase*>::iterator it = m_stateContainerList.begin(); it != m_stateContainerList.end(); ++it )
        (*it)->clearStates();
}
