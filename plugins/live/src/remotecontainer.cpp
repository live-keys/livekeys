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

#include "remotecontainer.h"

namespace lv{

RemoteContainer::RemoteContainer(QObject *parent)
    : QObject(parent)
    , m_componentComplete(false)
{

}

RemoteContainer::~RemoteContainer(){
}

void RemoteContainer::componentComplete(){
    m_componentComplete = true;
}

void RemoteContainer::sendError(const QByteArray &, int, const QString &){}
void RemoteContainer::sendBuild(const QByteArray &){}
void RemoteContainer::sendInput(const MLNode&){}

bool RemoteContainer::isReady() const{ return false; }

void RemoteContainer::onMessage(std::function<void (const LineMessage &, void*)>, void*){}
void RemoteContainer::onError(std::function<void (int, const std::string &)>){}

}// namespace
