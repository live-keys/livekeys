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

#include "remotelineresponse.h"
#include "tcplinesocket.h"

namespace lv{

RemoteLineResponse::RemoteLineResponse(QObject *parent)
    : QObject(parent)
{
}

void RemoteLineResponse::onResponse(std::function<void (const QString &, const QVariant &)> callback){
    m_responseCallback = callback;
}

void RemoteLineResponse::send(const QString &propertyName, const QVariant &value){
    m_responseCallback(propertyName, value);
}

}// namespace
