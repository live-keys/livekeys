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

#include "visuallognetworksender.h"
#include "live/mlnodetojson.h"
#include <QTcpSocket>
#include <QTimer>

/**
 * \class lv::VisualLogNetworkSender
 * \brief Implementation of VisualLog::Transport used for sending log messages over a network
 * \ingroup lvview
 */

namespace lv{

int VisualLogNetworkSender::RECONNECT_TIMEOUT = 5000;
int VisualLogNetworkSender::MESSAGE_QUEUE_SIZE = 10000;

/** Default constructor, initialized with an IP address and an appropriate port */
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

/** Default destructor */
VisualLogNetworkSender::~VisualLogNetworkSender(){
    delete m_timer;
}

/** QString variant of the sendMessage function */
void VisualLogNetworkSender::sendMessage(const QString &message){
    sendMessage(message.toUtf8());
}

/** Main, byte-array based function for sending messages over the network */
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

/** Standard string variant of the sendMessage function */
void VisualLogNetworkSender::sendMessage(const std::string &message){
    sendMessage(QByteArray::fromStdString(message));
}

/** Function that tries to connect to host a limited ammount of times */
void VisualLogNetworkSender::connectToHost(int maxRetries){
    m_connectRetries    = 0;
    m_connectMaxRetries = maxRetries;
    reconnect();
}

/** Sends a string-based log message over the network */
void VisualLogNetworkSender::onMessage(
        const VisualLog::Configuration *configuration,
        const VisualLog::MessageInfo &messageInfo,
        const std::string &message)
{
    sendMessage(messageInfo.prefix(configuration) + message);
}


/** Sends an object-based log entry over the network by converting it into JSON */
void VisualLogNetworkSender::onObject(
        const VisualLog::Configuration *configuration,
        const VisualLog::MessageInfo &messageInfo,
        const std::string &type,
        const MLNode &node)
{
    std::string nodestr;
    ml::toJson(node, nodestr);
    std::string pref = messageInfo.prefix(configuration);
    QByteArray messageData =
        QByteArray() + pref.c_str() + "\\@" + type.c_str() + "\n" +
        QString(static_cast<int>(pref.size()), ' ').toUtf8() + QByteArray::fromStdString(nodestr) + "\n";
    sendMessage(messageData);
}

/** When socket is connected, we send all the queued messages through */
void VisualLogNetworkSender::socketConnected(){
    m_connectRetries = 0;
    while ( !m_messageQueue.isEmpty() ){
        m_socket->write(m_messageQueue.dequeue() + "\n");
    }
}

/** When socket is disconnected. we notify the user of a failed connecting attempt, or finally notify that we gave up */
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

/** In case of socket error, we notify the user and try again if we have any attempts left */
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

/** Function that attempts to connect to host if we haven't exceeded the maximum number of attempts */
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
