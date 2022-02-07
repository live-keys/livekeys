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

#include "eventrelay.h"

#include <QCoreApplication>
#include <QKeyEvent>

namespace lv{

EventRelay::EventRelay(QObject *parent)
    : QObject(parent)
{
}

void EventRelay::relayKeyEvent(QObject *object, int key, int modifiers){
    QKeyEvent event(QKeyEvent::KeyPress, key, static_cast<Qt::KeyboardModifiers>(modifiers));
    QCoreApplication::sendEvent(object, &event);
}

}// namespace
