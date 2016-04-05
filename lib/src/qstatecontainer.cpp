/****************************************************************************
**
** Copyright (C) 2014 Dinu SV.
** (contact: mail@dinusv.com)
** This file is part of Live CV application.
**
** GNU General Public License Usage
** 
** This file may be used under the terms of the GNU General Public License 
** version 3.0 as published by the Free Software Foundation and appearing 
** in the file LICENSE.GPL included in the packaging of this file.  Please 
** review the following information to ensure the GNU General Public License 
** version 3.0 requirements will be met: http://www.gnu.org/copyleft/gpl.html.
**
****************************************************************************/

#include "qstatecontainer.h"
#include <QQuickView>
#include <QQuickItem>

QStateContainerManager* QStateContainerManager::m_instance = 0;

/*!
  \class QStateContainerManager
  \brief Manages state containers and assures their deletion
  \inmodule lcvlib_cpp
 */

/*!
  \brief QStateContainerManager singleton instance.

  Parameters:
  \a item
  \a parent
 */
QStateContainerManager &QStateContainerManager::instance(QQuickItem *item, QObject* parent){
    if ( m_instance == 0 )
        m_instance = new QStateContainerManager(item, parent);
    return *m_instance;
}

/*!
  \brief QStateContainerManager constructor

  Parameters:
  \a item
  \a parent
 */
QStateContainerManager::QStateContainerManager(QQuickItem *item, QObject *parent)
    : QObject(parent){

    atexit(&cleanStateManager);

    connect(item, SIGNAL(windowChanged(QQuickWindow*)), SLOT(attachWindow(QQuickWindow*)));
}

/*!
  \brief Calls beforeCompile() on all of its children
 */
void QStateContainerManager::beforeCompile(){
    for ( QLinkedList<QStateContainerBase*>::iterator it = m_stateContainerList.begin(); it != m_stateContainerList.end(); ++it )
        (*it)->beforeCompile();
}

/*!
  \brief Calls afterCompile() on all of its children
 */
void QStateContainerManager::afterCompile(){
    for ( QLinkedList<QStateContainerBase*>::iterator it = m_stateContainerList.begin(); it != m_stateContainerList.end(); ++it )
        (*it)->afterCompile();
}

/*!
  \brief Called when the gui initializes a window

  Parameters:
  \a window
 */
void QStateContainerManager::attachWindow(QQuickWindow *window){
    if (window != 0){
        QQuickView* view = qobject_cast<QQuickView*>(window);
        if ( view == 0 )
            qWarning("Cannot connect to window compilation signal. State objects will not get destroyed, "
                     "which will cause the application to slow down over time.");
        else {
            connect(view->rootObject(), SIGNAL(beforeCompile()), SLOT(beforeCompile()));
            connect(view->rootObject(), SIGNAL(afterCompile()),  SLOT(afterCompile()));
        }
    }
}

/*!
  \brief QStateContainerManager destructor
 */
QStateContainerManager::~QStateContainerManager(){
    for ( QLinkedList<QStateContainerBase*>::iterator it = m_stateContainerList.begin(); it != m_stateContainerList.end(); ++it )
        delete *it;
    m_stateContainerList.clear();
}

/*!
  \brief Register a state container to the manager.

  Parameters :
  \a container
 */
void QStateContainerManager::registerStateContainer(QStateContainerBase *container){
    m_stateContainerList.append(container);
}

/*!
  \brief Cleanup function.
 */
void QStateContainerManager::cleanStateManager() {
    delete m_instance;
    m_instance = 0;
}

/*!
  \class QStateContainerBase
  \brief Abstract state container type
  \inmodule lcvlib_cpp
 */

/*!
  \fn QStateContainerBase::QStateContainerBase()
  \brief QStateContainerBase constructor
 */

/*!
  \fn QStateContainerBase::~QStateContainerBase()
  \brief QStateContainerBase virtual destructor
 */

QStateContainerBase::QStateContainerBase(){
}

/*!
  \fn virtual void QStateContainerBase::beforeCompile()
  \brief Before compile method implementation.
 */

/*!
  \fn virtual void QStateContainerBase::afterCompile()
  \brief After compile method implementation.
 */

/*!
  \class QStateContainer
  \brief Actual state container implementation
  \inmodule lcvlib_cpp
 */

/*!
  \fn QStateContainer::QStateContainer(QQuickItem* item)
  \brief QStateContainer constructor
 */

/*!
  \fn QStateContainer::~QStateContainer()
  \brief QStateContainer constructor
 */

/*!
  \fn QStateContainer& QStateContainer::instance(QQuickItem* item)

  \brief QStateContainer singleton instance getter.

  Parameters :
  \a item
 */

/*!
  \fn virtual void QStateContainer::beforeCompile()
  \brief Before compile method implementation.
 */

/*!
  \fn virtual void QStateContainer::afterCompile()
  \brief After compile method implementation.
 */

/*!
  \fn virtual void QStateContainer::registerState(const QString& key, T* state)
  \brief Registers an objects state.

  Parameters:
  \a key
  \a state
 */

/*!
  \fn virtual void QStateContainer::state(const QString& key)
  \brief Retrieves an objects state. Returns 0 if none was found with that key.

  Parameters:
  \a key
 */
