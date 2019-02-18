#ifndef LVTCPAGENT_H
#define LVTCPAGENT_H

#include <QObject>
#include <QAbstractSocket>
#include <QQmlParserStatus>
#include <QQmlPropertyMap>

class QTimer;
class QTcpSocket;
class QQmlComponent;
class QQmlContext;

namespace lv{

/// \private
class TcpAgent : public QObject, public QQmlParserStatus{

    Q_OBJECT
    Q_INTERFACES(QQmlParserStatus)
    Q_PROPERTY(QString address READ address WRITE setAddress NOTIFY addressChanged)
    Q_PROPERTY(int port        READ port    WRITE setPort    NOTIFY portChanged)

public:
    static int RECONNECT_TIMEOUT;

public:
    explicit TcpAgent(QObject *parent = 0);
    ~TcpAgent();

    void connectToHost();
    void sendError(const QByteArray& type, int code, const QString& message);

    int port() const;
    const QString& address() const;

    void setAddress(const QString& adress);
    void setPort(int port);

    void classBegin() Q_DECL_OVERRIDE{}
    void componentComplete() Q_DECL_OVERRIDE;
signals:
    void addressChanged();
    void portChanged();

public slots:
    void socketConnected();
    void socketDisconnected();
    void socketError(QAbstractSocket::SocketError);
    void reconnect();
    void socketData();
    void outputValueChanged(const QString& key, const QVariant& value);

private:
    QTimer* timer();

    QByteArray         m_collectedData;
    QTcpSocket*        m_socket;
    QString            m_address;
    int                m_port;
    QTimer*            m_timer;
    QQmlComponent*     m_component;
    QQmlContext*       m_componentContext;
    bool               m_componentComplete;

    QQmlPropertyMap*   m_root;
    QQmlPropertyMap*   m_input;
    QQmlPropertyMap*   m_output;
    QObject*           m_sourceItem;
};

inline const QString& TcpAgent::address() const{
    return m_address;
}

inline int TcpAgent::port() const{
    return m_port;
}

inline void TcpAgent::setAddress(const QString &address){
    if (m_address == address)
        return;

    m_address = address;
    emit addressChanged();
    connectToHost();
}

inline void TcpAgent::setPort(int port){
    if (m_port == port)
        return;

    m_port = port;
    emit portChanged();
    connectToHost();
}

}// namespace

#endif // LVTCPAGENT_H
