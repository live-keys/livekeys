#include "tcplinesocket.h"
#include "live/viewengine.h"
#include "live/viewcontext.h"
#include "live/exception.h"
#include "live/mlnode.h"
#include "live/mlnodetojson.h"
#include <QTcpSocket>
#include <QHostAddress>

namespace lv{

TcpLineSocket::TcpLineSocket(QTcpSocket *socket, QObject *parent)
    : QObject(parent)
    , m_socket(socket)
    , m_initialized(false)
{
    connect(m_socket, SIGNAL(error(QAbstractSocket::SocketError)),
            this,     SLOT(tcpError(QAbstractSocket::SocketError)));
    m_socket->setSocketOption(QAbstractSocket::KeepAliveOption, true);
    m_address = m_socket->peerAddress().toString();
}

TcpLineSocket::~TcpLineSocket(){
    if ( m_socket->state() == QTcpSocket::ConnectedState ){
        m_socket->disconnectFromHost();
        m_socket->waitForDisconnected(5000);
    }
    delete m_socket;
}

void TcpLineSocket::initialize(const MLNode& data){
    QByteArray jsonResult;
    ml::toJson(data, jsonResult);
    m_socket->write(jsonResult);
}

void TcpLineSocket::sendInput(const MLNode &n){
    QByteArray jsonResult;
    ml::toJson(n, jsonResult);
    m_socket->write(jsonResult);
}

QByteArray TcpLineSocket::readAll(){
    return m_socket->readAll();
}

void TcpLineSocket::tcpError(QAbstractSocket::SocketError){
    lv::Exception e = CREATE_EXCEPTION(
        lv::Exception, "Log listener socket error: " + m_socket->errorString().toStdString(), 0
    );
    lv::ViewContext::instance().engine()->throwError(&e);
}



}// namespace
