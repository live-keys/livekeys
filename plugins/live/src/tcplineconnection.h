#ifndef TCPLINECONNECTION_H
#define TCPLINECONNECTION_H

#include <QObject>
#include <QQmlParserStatus>
#include <QAbstractSocket>

#include "live/lvliveglobal.h"
#include "live/linecapture.h"
#include "remotecontainer.h"

class QTimer;
class QTcpSocket;

namespace lv{

class MLNode;

class LV_LIVE_EXPORT TcpLineConnection : public RemoteContainer{

    Q_OBJECT
    Q_PROPERTY(QString address READ address WRITE setAddress NOTIFY addressChanged)
    Q_PROPERTY(int port        READ port    WRITE setPort    NOTIFY portChanged)

public:
    static int RECONNECT_TIMEOUT;
    static const int DEFAULT_PORT = 1591;

public:
    explicit TcpLineConnection(QObject *parent = nullptr);
    ~TcpLineConnection();

    void connectToHost();

    void sendError(const QByteArray& type, int code, const QString& message);
    void sendBuild(const QByteArray& buildData);
    void sendInput(const MLNode& input);

    void onMessage(std::function<void(const LineMessage&, void* data)> handler, void* handlerData = 0);
    void onError(std::function<void(int, const std::string&)> handler);

    QString address() const;
    int port() const;

    bool isReady() const;

protected:
    void classBegin() Q_DECL_OVERRIDE{}
    void componentComplete() Q_DECL_OVERRIDE;

signals:
    void addressChanged();
    void portChanged();

public slots:
    void socketConnected();
    void socketDisconnected();
    void socketError(QAbstractSocket::SocketError);
    void socketData();
    void reconnect();

    void setAddress(const QString &address);
    void setPort(int port);

private:
    QTimer* timer();

    QString     m_address;
    QTcpSocket* m_socket;
    int         m_port;
    QTimer*     m_timer;

    LineCapture m_dataCapture;
};

inline QString TcpLineConnection::address() const{
    return m_address;
}

inline int TcpLineConnection::port() const{
    return m_port;
}

inline void TcpLineConnection::setAddress(const QString& address){
    if (m_address == address)
        return;

    m_address = address;
    emit addressChanged();
}

inline void TcpLineConnection::setPort(int port){
    if (m_port == port)
        return;

    m_port = port;
    emit portChanged();
}

}// namespace

#endif // TCPLINECONNECTION_H
