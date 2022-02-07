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

#ifndef LVLOGLISTENERSOCKET_H
#define LVLOGLISTENERSOCKET_H

#include <QObject>
#include <QAbstractSocket>
#include "live/visuallog.h"

class QTcpSocket;

namespace lv{

/// \private
class LogListenerSocket : public QObject{

    Q_OBJECT

public:
    const int MinimumPrefixSize = 34;

    explicit LogListenerSocket(QTcpSocket* socket, QObject *parent = nullptr);
    ~LogListenerSocket();

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

inline const QString &LogListenerSocket::address() const{
    return m_address;
}

} // namespace

#endif // LVLOGLISTENERSOCKET_H
