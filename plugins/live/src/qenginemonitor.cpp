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

#include "qenginemonitor.h"
#include <QQuickItem>
#include <QQmlContext>
#include <QQmlEngine>
#include <QVariant>

/*!
   \class lv::EngineMonitor
   \inmodule live_cpp
   \internal
 */

/*!
  \qmltype engineMonitor
  \instantiates lcv::QEngineMonitor
  \inqmlmodule live
  \inherits QtObject
  \brief Provides engine signals. You can use the \c engineMonitor object to trigger different behaviors when
  before or after the engine compiles the code.
 */

/*!
  \qmlsignal engineMonitor::beforeCompile()
  This signal is emited before a new compilation is started by the engine.
 */

/*!
  \qmlsignal engineMonitor::afterCompile()
  This signal is emited after a succesfull compilation.
 */

/*!
  \qmlsignal engineMonitor::targetChanged()
  This signal is emited when the compilation target changed, either by opening a new project, or by setting
  changing the active file within a project.
 */

/*!
   \class lv::EngineMonitor
   \inmodule live_cpp
   \brief Provides engine signals.

   You can acquire the engine monitor through the context of your current item:

   \code
   QEngineMonitor* emonitor = QEngineMonitor::grabFromContext(this);
   \endcode

   Note that the context will not be available during construction of your item, and is recommended to be used
   after the component has been completed.
 */


/*!
  \brief lcv::QEngineMonitor::QEngineMonitor
  \a parent
 */
QEngineMonitor::QEngineMonitor(QObject *parent)
    : QObject(parent)
{
}

/*!
  \brief QEngineMonitor::grabFromContext
  \a item
  \a contextProperty

  Returns the engine monitor located within the items context.
 */
QEngineMonitor *QEngineMonitor::grabFromContext(QQuickItem *item, const QString &contextProperty){
    QQmlContext* ctx = qmlContext(item);
    if ( ctx == 0 ){
        qWarning("Context is not yet available. Call this method outside the constructor of your current object.");
        return 0;
    }
    return static_cast<QEngineMonitor*>(ctx->contextProperty(contextProperty).value<QObject*>());
}
