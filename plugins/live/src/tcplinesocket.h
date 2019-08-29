#ifndef LVTCPLINESOCKET_H
#define LVTCPLINESOCKET_H

#include "live/lvliveglobal.h"
#include "live/linecapture.h"
#include "live/exception.h"

#include <QObject>
#include <QAbstractSocket>
#include <QQmlPropertyMap>

class QTcpSocket;
class QQmlComponent;
class QQmlContext;

namespace lv{

class MLNode;
class TcpLineServer;
class RemoteLineResponse;

class LV_LIVE_EXPORT TcpLineSocket : public QObject{

    Q_OBJECT

public:
    TcpLineSocket(QTcpSocket* socket, QObject *parent = nullptr);
    ~TcpLineSocket();

    const QString& address();

    static void receiveMessage(const LineMessage& message, void* data);
    void onMessage(const LineMessage& message);

    QQmlPropertyMap* post() const;

    void sendError(const QByteArray& type, Exception::Code code, const QString& message);

public slots:
    void responseValueChanged(const QString& key, const QVariant& value);
    void tcpError(QAbstractSocket::SocketError error);
    void tcpRead();

private:
    TcpLineServer* server();

    QTcpSocket* m_socket;
    QString     m_address;
    bool        m_initialized;

    LineCapture         m_lineCapture;

    QQmlPropertyMap*    m_post;
    RemoteLineResponse* m_response;
    QQmlComponent*      m_component;
    QQmlContext*        m_componentContext;
    QObject*            m_sourceItem;
};

inline const QString &TcpLineSocket::address(){
    return m_address;
}

inline QQmlPropertyMap *TcpLineSocket::post() const{
    return m_post;
}

}// namespace

#endif // LVTCPLINESOCKET_H
