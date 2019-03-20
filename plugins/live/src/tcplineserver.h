#ifndef TCPLINESERVER_H
#define TCPLINESERVER_H

#include "live/lvviewglobal.h"
#include "live/shared.h"
#include "live/exception.h"
#include <QObject>
#include <QQmlParserStatus>
#include <QVariant>

class QTcpServer;
class QTcpSocket;

namespace lv{

class TcpLineSocket;
class TcpLineServer : public QObject, public QQmlParserStatus{

    Q_OBJECT
    Q_INTERFACES(QQmlParserStatus)
    Q_PROPERTY(QString address READ address WRITE setAddress NOTIFY addressChanged)
    Q_PROPERTY(int port        READ port    WRITE setPort    NOTIFY portChanged)

    friend class TcpLineSocket;

public:
    TcpLineServer(QObject* parent = nullptr);
    ~TcpLineServer();

    QVariant result() const;
    void setResult(QVariant result);

    bool isComponentComplete();

    QString address() const;
    int port() const;

    void setAddress(const QString& address);
    void setPort(int port);


public slots:
    void newConnection();

signals:
    void resultChanged();
    void addressChanged();
    void portChanged();

    void complete();
    void listening();
    void error(const QByteArray& type, int code, const QString& message);

protected:
    void classBegin(){}
    void componentComplete();

private:
    void startListening();
    void removeSocket(TcpLineSocket *socket);
    void lineSocketError(TcpLineSocket* socket, const QByteArray& type, Exception::Code code, const QString& message);


    bool     m_isComponentComplete;
    QVariant m_result;
    QString  m_address;
    int      m_port;

    QTcpServer*           m_server;
    QList<TcpLineSocket*> m_sockets;
};

inline QVariant TcpLineServer::result() const{
    return m_result;
}

inline void TcpLineServer::setResult(QVariant result){
    if (m_result == result)
        return;

    m_result = result;
    emit resultChanged();
}

inline QString TcpLineServer::address() const{
    return m_address;
}

inline int TcpLineServer::port() const{
    return m_port;
}

inline void TcpLineServer::setAddress(const QString &address){
    if (m_address == address)
        return;

    m_address = address;
    emit addressChanged();
}

inline void TcpLineServer::setPort(int port){
    if (m_port == port)
        return;

    m_port = port;
    emit portChanged();
}

}// namespace

#endif // TCPLINESERVER_H
