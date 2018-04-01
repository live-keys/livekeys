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

#ifndef LVVISUALLOGNETWORKSENDER_H
#define LVVISUALLOGNETWORKSENDER_H

#include <QObject>
#include <QQueue>
#include <QAbstractSocket>
#include "live/lvbaseglobal.h"
#include "live/visuallog.h"

class QTimer;
class QTcpSocket;

namespace lv{

class LV_BASE_EXPORT VisualLogNetworkSender : public QObject, public VisualLog::Transport{

    Q_OBJECT

public:
    static int RECONNECT_TIMEOUT;
    static int MESSAGE_QUEUE_SIZE;

public:
    explicit VisualLogNetworkSender(const QString& ip, int port, QObject *parent = 0);
    ~VisualLogNetworkSender();

    void sendMessage(const QString& message);
    void sendMessage(const QByteArray& message);
    void connectToHost(int maxRetries = 5);

    // Transport interface
public:
    void onMessage(
        const VisualLog::Configuration *configuration,
        const VisualLog::MessageInfo &messageInfo,
        const QString &message) Q_DECL_OVERRIDE;
    void onObject(
        const VisualLog::Configuration *configuration,
        const VisualLog::MessageInfo &messageInfo,
        const QString &type,
        const MLNode &node) Q_DECL_OVERRIDE;

public slots:
    void socketConnected();
    void socketDisconnected();
    void socketError(QAbstractSocket::SocketError);
    void reconnect();

private:
    QTimer* timer();

    QQueue<QByteArray> m_messageQueue;
    QTcpSocket*        m_socket;
    QString            m_ip;
    int                m_port;
    QTimer*            m_timer;
    int                m_connectRetries;
    int                m_connectMaxRetries;
};

}// namespace

#endif // LVVISUALLOGNETWORKSENDER_H
