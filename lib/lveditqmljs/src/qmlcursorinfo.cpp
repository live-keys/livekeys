/****************************************************************************
**
** Copyright (C) 2014-2019 Dinu SV.
** (contact: mail@dinusv.com)
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

#include "qmlcursorinfo.h"

namespace lv{

QmlCursorInfo::QmlCursorInfo(bool canBind, bool canUnbind, bool canEdit, bool canAdjust, bool canShape, QObject *parent)
    : QObject(parent)
    , m_canBind(canBind)
    , m_canUnbind(canUnbind)
    , m_canEdit(canEdit)
    , m_canAdjust(canAdjust)
    , m_canShape(canShape)
{
}

QmlCursorInfo::QmlCursorInfo(QObject *parent)
    : QObject(parent)
    , m_canBind(false)
    , m_canUnbind(false)
    , m_canEdit(false)
    , m_canAdjust(false)
    , m_canShape(false)
{
}

QmlCursorInfo::~QmlCursorInfo(){
}


}// namespace
