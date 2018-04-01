/****************************************************************************
**
** Copyright (C) 2014-2018 Dinu SV.
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

#include "visuallognetworksender.h"
#include "live/mlnodetojson.h"
#include <QTcpSocket>
#include <QTimer>

namespace lv{

int VisualLogNetworkSender::RECONNECT_TIMEOUT = 5000;
int VisualLogNetworkSender::MESSAGE_QUEUE_SIZE = 10000;

VisualLogNetworkSender::VisualLogNetworkSender(const QString &ip, int port, QObject *parent)
    : QObject(parent)
    , m_socket(new QTcpSocket(this))
    , m_ip(ip)
    , m_port(port)
    , m_timer(0)
    , m_connectRetries(0)
    , m_connectMaxRetries(5)
{
    connect(m_socket, SIGNAL(connected()), this, SLOT(socketConnected()));
    connect(m_socket, SIGNAL(disconnected()), this, SLOT(socketDisconnected()));
    connect(m_socket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(socketError(QAbstractSocket::SocketError)));
}

VisualLogNetworkSender::~VisualLogNetworkSender(){
    delete m_timer;
}

void VisualLogNetworkSender::sendMessage(const QString &message){
    sendMessage(message.toUtf8());
}

void VisualLogNetworkSender::sendMessage(const QByteArray &message){
    if ( m_socket->state() == QTcpSocket::ConnectedState){
        m_socket->write(message + "\n");
    } else if ( m_socket->state() == QTcpSocket::UnconnectedState ){
        reconnect();
    } else {
        m_messageQueue.enqueue(message);
        if ( m_messageQueue.size() >= MESSAGE_QUEUE_SIZE )
            m_messageQueue.dequeue();
    }
}

void VisualLogNetworkSender::connectToHost(int maxRetries){
    m_connectRetries    = 0;
    m_connectMaxRetries = maxRetries;
    reconnect();
}

void VisualLogNetworkSender::onMessage(
        const VisualLog::Configuration *configuration,
        const VisualLog::MessageInfo &messageInfo,
        const QString &message)
{
    sendMessage(messageInfo.prefix(configuration) + message);
}

void VisualLogNetworkSender::onObject(
        const VisualLog::Configuration *configuration,
        const VisualLog::MessageInfo &messageInfo,
        const QString &type,
        const MLNode &node)
{
    QByteArray nodestr;
    ml::toJson(node, nodestr);
    QString pref = messageInfo.prefix(configuration);
    QByteArray messageData =
        pref.toUtf8() + "\\@" + type.toUtf8() + "\n" +
        QString(pref.length(), ' ').toUtf8() + nodestr + "\n";
    sendMessage(messageData);
}

void VisualLogNetworkSender::socketConnected(){
    m_connectRetries = 0;
    while ( !m_messageQueue.isEmpty() ){
        m_socket->write(m_messageQueue.dequeue() + "\n");
    }
}

void VisualLogNetworkSender::socketDisconnected(){
    if ( m_connectRetries < m_connectMaxRetries ){
        qWarning(
            "Disconnected from %s: \'%s\'. [%d] Retrying...", qPrintable(m_ip), qPrintable(m_socket->errorString()), m_connectRetries
        );
        timer()->start(RECONNECT_TIMEOUT);
    } else {
        qWarning("Disconnected from %s: \'%s\'.", qPrintable(m_ip), qPrintable(m_socket->errorString()));
    }
}

void VisualLogNetworkSender::socketError(QAbstractSocket::SocketError){
    m_socket->abort();
    if ( m_connectRetries < m_connectMaxRetries ){
        qWarning(
            "Failed to connect to %s: \'%s\'. [%d] Reconnecting...",
            qPrintable(m_ip), qPrintable(m_socket->errorString()), m_connectRetries
        );
        timer()->start(RECONNECT_TIMEOUT);
    } else {
        qWarning("Failed to connect to %s: \'%s\'", qPrintable(m_ip), qPrintable(m_socket->errorString()));
    }
}

void VisualLogNetworkSender::reconnect(){
    if ( m_connectRetries < m_connectMaxRetries ){
        m_socket->connectToHost(m_ip, m_port, QTcpSocket::WriteOnly);
        ++m_connectRetries;
    }
}

QTimer *VisualLogNetworkSender::timer(){
    if ( !m_timer ){
        m_timer = new QTimer;
        m_timer->setSingleShot(true);
        connect(m_timer, SIGNAL(timeout()), this, SLOT(reconnect()));
    }

    return m_timer;
}

}// namespace
