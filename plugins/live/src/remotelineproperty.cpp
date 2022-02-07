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

#include "remotelineproperty.h"
#include "remoteline.h"

namespace lv{

RemoteLineProperty::RemoteLineProperty(const QString& name, RemoteLine *parent)
    : QObject(parent)
    , m_line(parent)
    , m_name(name)
{
}

void RemoteLineProperty::changed(){

}

}// namespace
