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

#ifndef LVLOGLISTENER_H
#define LVLOGLISTENER_H

#include <QObject>
#include <QQmlParserStatus>

class QTcpServer;

namespace lv{

/// \private
class LogListener : public QObject, public QQmlParserStatus{

    Q_OBJECT
    Q_INTERFACES(QQmlParserStatus)
    Q_PROPERTY(QString address READ address WRITE setAddress NOTIFY addressChanged)
    Q_PROPERTY(int     port    READ port    WRITE setPort    NOTIFY portChanged)
    Q_PROPERTY(QStringList allowedIps READ allowedIps WRITE setAllowedIps NOTIFY allowedIpsChanged)

public:
    explicit LogListener(QObject *parent = nullptr);
    ~LogListener();

    QString address() const;
    int port() const;
    const QStringList& allowedIps() const;

    void setAddress(QString address);
    void setPort(int port);
    void setAllowedIps(const QStringList& allowedIps);

    void componentComplete();
    void classBegin(){}

public slots:
    void newConnection();

signals:
    void addressChanged();
    void portChanged();
    void allowedIpsChanged();
    void listening();

private:
    void startListening();

    QString     m_address;
    int         m_port;
    QStringList m_allowedIps;
    QTcpServer* m_server;

    bool        m_componentComplete;
};

inline QString LogListener::address() const{
    return m_address;
}

inline int LogListener::port() const{
    return m_port;
}

inline const QStringList &LogListener::allowedIps() const{
    return m_allowedIps;
}

}// namespace

#endif // LVLOGLISTENER_H
