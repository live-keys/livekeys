#include "qloglistener.h"
#include "qloglistenersocket.h"
#include "live/plugincontext.h"
#include "live/exception.h"
#include "live/engine.h"

#include <QHostAddress>
#include <QTcpServer>
#include <QTcpSocket>

QLogListener::QLogListener(QObject *parent)
    : QObject(parent)
    , m_port(1590)
    , m_server(new QTcpServer(this))
    , m_componentComplete(false)
{
    connect(m_server, SIGNAL(newConnection()), this, SLOT(newConnection()));
}

QLogListener::~QLogListener(){
}

void QLogListener::setAddress(QString address){
    if (m_address == address)
        return;

    m_address = address;
    emit addressChanged();

    startListening();
}

void QLogListener::setPort(int port){
    if (m_port == port)
        return;

    m_port = port;
    emit portChanged();

    startListening();
}

void QLogListener::componentComplete(){
    m_componentComplete = true;
    startListening();
}

void QLogListener::newConnection(){
    QTcpSocket *socket = m_server->nextPendingConnection();
    if ( m_allowedIps.isEmpty() || m_allowedIps.contains(socket->peerAddress().toString()) ){
        QLogListenerSocket* logListener = new QLogListenerSocket(socket, this);
        connect(socket, SIGNAL(disconnected()), logListener, SLOT(deleteLater()));
    } else {
        socket->disconnect();
        socket->waitForDisconnected();
        delete socket;
    }
}

void QLogListener::startListening(){
    if ( !m_componentComplete )
        return;

    if ( m_server->isListening() ){
        m_server->close();
    }

    if ( m_address == "" ){
        if( !m_server->listen(QHostAddress::Any, m_port) ){
            lv::Exception e = CREATE_EXCEPTION(lv::Exception, "Cannot open tcp connection.", 0);
            lv::PluginContext::engine()->throwError(&e);
            return;
        }
        m_address = m_server->serverAddress().toString();
        emit listening();
    } else {
        if( !m_server->listen(QHostAddress(m_address), m_port) ){
            lv::Exception e = CREATE_EXCEPTION(lv::Exception, "Cannot open tcp connection to " + m_address + ".", 0);
            lv::PluginContext::engine()->throwError(&e);
            return;
        }
        emit listening();
    }
}

void QLogListener::setAllowedIps(const QStringList& allowedIps){
    if (m_allowedIps == allowedIps)
        return;

    m_allowedIps = allowedIps;
    emit allowedIpsChanged();

    //TODO: Cleanup connections
}
