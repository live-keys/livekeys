#ifndef TCPLINESOCKET_H
#define TCPLINESOCKET_H

#include <QObject>
#include <QAbstractSocket>

class QTcpSocket;

namespace lv{

class MLNode;

class TcpLineSocket : public QObject{

    Q_OBJECT

public:
    explicit TcpLineSocket(QTcpSocket* socket, QObject *parent = nullptr);
    ~TcpLineSocket();

    const QString& address() const;

    void initialize(const MLNode& data);
    void sendInput(const MLNode& n);
    bool isInitialized() const;

    QByteArray readAll();

signals:
    void outputReady();

public slots:
    void tcpError(QAbstractSocket::SocketError error);

private:
    QTcpSocket* m_socket;
    QString     m_address;
    bool        m_initialized;
};

inline const QString& TcpLineSocket::address() const{
    return m_address;
}

inline bool TcpLineSocket::isInitialized() const{
    return m_initialized;
}

} // namespace

#endif // TCPLINESOCKET_H
