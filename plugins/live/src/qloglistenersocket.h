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

#ifndef QLOGLISTENERSOCKET_H
#define QLOGLISTENERSOCKET_H

#include <QObject>
#include <QAbstractSocket>
#include "live/visuallog.h"

class QTcpSocket;
class QLogListenerSocket : public QObject{

    Q_OBJECT

public:
    const int MinimumPrefixSize = 34;

    explicit QLogListenerSocket(QTcpSocket* socket, QObject *parent = nullptr);
    ~QLogListenerSocket();

    const QString& address() const;

public slots:
    void tcpRead();
    void tcpError(QAbstractSocket::SocketError error);

private:
    class ObjectMessageInfo;

    bool isPrefix(
        const QByteArray& buffer,
        int dateIndex,
        int& levelIndex,
        int& functionIndex,
        int& lineIndex,
        int& separatorIndex
    );
    int isIp(const QByteArray& buffer);
    void logLine(const QByteArray& buffer);
    void logLine(lv::VisualLog& vl, const QByteArray& buffer);

    QTcpSocket* m_socket;
    QString     m_address;
    QByteArray  m_buffer;
    ObjectMessageInfo* m_expectObject;
};

inline const QString &QLogListenerSocket::address() const{
    return m_address;
}

#endif // QLOGLISTENERSOCKET_H
