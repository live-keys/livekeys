/****************************************************************************
**
** Copyright (C) 2015-2016 Dinu SV.
** (contact: mail@dinusv.com)
** This file is part of QML Gantt library.
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

#include "qganttmodelitem.h"
#include <QDebug>

QGanttModelItem::QGanttModelItem(QObject *parent)
    : QObject(parent)
    , m_position(0)
    , m_length(0)
{
}

QGanttModelItem::QGanttModelItem(qint64 position, qint64 length, QObject* parent)
    : QObject(parent)
    , m_position(position)
    , m_length(length)
{
}

QGanttModelItem::~QGanttModelItem(){
    if ( !m_data.isNull() && m_data.canConvert<QObject*>())
        delete m_data.value<QObject*>();
}
