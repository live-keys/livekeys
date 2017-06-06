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

namespace lcv{

QEngineMonitor::QEngineMonitor(QObject *parent)
    : QObject(parent)
{
}

QEngineMonitor *QEngineMonitor::grabFromContext(QQuickItem *item, const QString &contextProperty){
    return static_cast<QEngineMonitor*>(qmlContext(item)->contextProperty(contextProperty).value<QObject*>());
}

}// namespace
