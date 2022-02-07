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

#include "tcplineserver.h"
#include "tcplinesocket.h"
#include "live/viewcontext.h"
#include "live/viewengine.h"
#include "live/exception.h"
#include "live/visuallogqt.h"

#include "live/mlnode.h"
#include "live/mlnodetojson.h"

#include "live/linemessage.h"
#include "live/linecapture.h"
#include "tcplineconnection.h"

#include <QQmlContext>
#include <QQmlEngine>
#include <QTcpServer>
#include <QTcpSocket>

namespace lv{

TcpLineServer::TcpLineServer(QObject* parent)
    : QObject(parent)
    , m_isComponentComplete(false)
    , m_port(TcpLineConnection::DEFAULT_PORT)
    , m_server(new QTcpServer)
{
    connect(m_server, &QTcpServer::newConnection, this, &TcpLineServer::newConnection);
}

TcpLineServer::~TcpLineServer(){
}

void TcpLineServer::newConnection(){
    QTcpSocket *socket        = m_server->nextPendingConnection();
    TcpLineSocket* lineSocket = new TcpLineSocket(socket, this);
    connect(socket, &QTcpSocket::disconnected, [this, lineSocket](){ removeSocket(lineSocket); } );
    connect(socket, SIGNAL(disconnected()), lineSocket, SLOT(deleteLater()));
    m_sockets.append(lineSocket);

    vlog("tcpline-server").v() << "New connection from :" << lineSocket->address();
}

void TcpLineServer::componentComplete(){
    m_isComponentComplete = true;
    emit complete();

    startListening();
}

void TcpLineServer::startListening(){
    if ( !m_isComponentComplete )
        return;

    if ( m_server->isListening() ){
        m_server->close();
    }

    if ( m_address == "" ){
        if( !m_server->listen(QHostAddress::Any, m_port) ){
            lv::Exception e = CREATE_EXCEPTION(
                lv::Exception, "TcpLineServer: Cannot open tcp connection.", Exception::toCode("~Connect")
            );
            lv::ViewContext::instance().engine()->throwError(&e);
            return;
        }
        m_address = m_server->serverAddress().toString();
        vlog_debug("lvview-tcpline", "Started listening on " + m_address + ":" + QString::number(m_port));

        emit listening();
    } else {
        if( !m_server->listen(QHostAddress(m_address), m_port) ){
            lv::Exception e = CREATE_EXCEPTION(
                lv::Exception, "TcpLineServer: Cannot open tcp connection to " + m_address.toStdString() + ".", Exception::toCode("~Connect")
            );
            lv::ViewContext::instance().engine()->throwError(&e);
            return;
        }

        vlog("tcpline-server").v() << "Started listening on " << m_address << ":" << m_port;

        emit listening();
    }
}

void TcpLineServer::removeSocket(TcpLineSocket *socket){
    for ( auto it = m_sockets.begin(); it != m_sockets.end(); ++it ){
        if ( *it == socket ){
            m_sockets.erase(it);
            return;
        }
    }
}

void TcpLineServer::lineSocketError(TcpLineSocket *, const QByteArray &type, Exception::Code code, const QString &message){
    emit error(type, code, message);
}

}// namespace
