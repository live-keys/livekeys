#include "tcplineconnection.h"

#include "live/exception.h"
#include "live/mlnode.h"
#include "live/mlnodetojson.h"
#include "live/visuallogqt.h"

#include <QQmlEngine>
#include <QTcpSocket>
#include <QTimer>

namespace lv{

int TcpLineConnection::RECONNECT_TIMEOUT = 5000;

TcpLineConnection::TcpLineConnection(QObject *parent)
    : QObject(parent)
    , m_componentComplete(false)
    , m_socket(new QTcpSocket(this))
    , m_port(TcpLineConnection::DEFAULT_PORT)
    , m_timer(nullptr)
{
    connect(m_socket, SIGNAL(connected()),    this, SLOT(socketConnected()));
    connect(m_socket, SIGNAL(disconnected()), this, SLOT(socketDisconnected()));
    connect(m_socket, SIGNAL(readyRead()),    this, SLOT(socketData()));
    connect(m_socket, SIGNAL(error(QAbstractSocket::SocketError)),
            this, SLOT(socketError(QAbstractSocket::SocketError)));
}

TcpLineConnection::~TcpLineConnection(){
    delete m_timer;
    if ( m_socket->state() == QTcpSocket::ConnectedState ){
        m_socket->disconnectFromHost();
        m_socket->waitForDisconnected(5000);
    }
    delete m_socket;
}

void TcpLineConnection::connectToHost(){
    if ( m_componentComplete && m_address != "" && m_port != -1 ){
        reconnect();
    }
}

void TcpLineConnection::sendError(const QByteArray &type, int code, const QString &message){
    MLNode errorObject = {
        {"code", code},
        {"message", message.toStdString()},
        {"type", type.toStdString()}
    };

    std::string errorSerialized;
    ml::toJson(errorObject, errorSerialized);

    QByteArray toSend = LineMessage::create(
        LineMessage::Error | LineMessage::Json,
        errorSerialized.c_str(),
        (int)errorSerialized.size()
    );
    m_socket->write(toSend);
}

void TcpLineConnection::sendBuild(const QByteArray &buildData){
    QByteArray toSend = LineMessage::create(
        LineMessage::Build | LineMessage::Raw,
        buildData
    );

    m_socket->write(toSend);
}

void TcpLineConnection::sendInput(const MLNode &input){
    std::string inputSerialized;
    ml::toJson(input, inputSerialized);

    QByteArray toSend = LineMessage::create(
        LineMessage::Input | LineMessage::Json,
        inputSerialized.c_str(),
        inputSerialized.size()
    );

    m_socket->write(toSend);
}

bool TcpLineConnection::isConnected() const{
    return m_socket->state() == QTcpSocket::ConnectedState;
}

void TcpLineConnection::componentComplete(){
    m_componentComplete = true;
    connectToHost();
}

void TcpLineConnection::socketConnected(){
    vlog("tcp-line-client").v() << "Connected to :" + m_address;

    emit connectionEstablished();
}

void TcpLineConnection::socketDisconnected(){
    vlog("tcp-line-client").w() << "Disconnected from " << m_address << m_socket->errorString();
}

void TcpLineConnection::socketError(QAbstractSocket::SocketError){
    m_socket->abort();

    vlog("tcp-line-client").w() << "Failed to connect to " << m_address << m_socket->errorString();

    timer()->start(RECONNECT_TIMEOUT);
}

void TcpLineConnection::socketData(){
    m_dataCapture.append(m_socket->readAll());
}

void TcpLineConnection::reconnect(){
    m_socket->connectToHost(m_address, m_port, QTcpSocket::ReadWrite);
}

QTimer *TcpLineConnection::timer(){
    if ( !m_timer ){
        m_timer = new QTimer;
        m_timer->setSingleShot(true);
        connect(m_timer, SIGNAL(timeout()), this, SLOT(reconnect()));
    }

    return m_timer;
}

}// namespace
