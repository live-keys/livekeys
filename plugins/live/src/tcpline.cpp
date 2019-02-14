#include "tcpline.h"
#include "tcplinesocket.h"
#include "live/filter.h"
#include "live/viewcontext.h"
#include "live/viewengine.h"
#include "live/exception.h"
#include "live/documentqmlinfo.h"
#include "live/lockedfileiosession.h"
#include "live/project.h"
#include "live/mlnode.h"
#include "live/mlnodetojson.h"

#include <QDebug>
#include <QQmlContext>
#include <QQmlEngine>
#include <QTcpServer>
#include <QTcpSocket>

namespace lv{

TcpLine::TcpLine(QObject *parent)
    : QObject(parent)
    , m_source(0)
    , m_input(0)
    , m_output(0)
    , m_project(0)
    , m_server(new QTcpServer(this))
    , m_port(1591)
    , m_componentComplete(false)
{
    m_project = qobject_cast<lv::Project*>(
        lv::ViewContext::instance().engine()->engine()->rootContext()->contextProperty("project").value<QObject*>()
    );
    if ( !m_project ){
        lv::Exception e = CREATE_EXCEPTION(lv::Exception, "Failed to load 'project' property from context", 0);
        lv::ViewContext::instance().engine()->throwError(&e);
    }
    connect(m_server, SIGNAL(newConnection()), this, SLOT(newConnection()));
}

TcpLine::~TcpLine(){
}

void TcpLine::setSource(ComponentSource *source){
    if ( source == m_source )
        return;

    m_source = source;

    initializeAgent();

    emit sourceChanged();
}

void TcpLine::setInput(Tuple *input){
    m_input = input;
    emit inputChanged();
    if ( !m_input )
        return;

    if ( isAgentInitialized() ){
        TcpLineSocket* tls = agent();
        if ( tls ){
            SharedDataLocker* locker = createLocker();
            if ( m_input->reserveForRead(locker, this)){

                MLNode result;
                Tuple::serialize(ViewContext::instance().engine(), *input, result);

                tls->sendInput(result);

            }
        }
    } else {
        initializeAgent();
    }
}

void TcpLine::setOutput(Tuple *output){
    m_output = output;
    emit outputChanged();

    initializeAgent();
}

void TcpLine::componentComplete(){
    m_componentComplete = true;

    startListening();
}

void TcpLine::process(){
    use(createLocker()->write(m_output),
        [this](){
            Tuple::deserialize(lv::ViewContext::instance().engine(), m_receivedOutput[""]["output"], *m_output);
        }, [this](){
            emit outputChanged();
        }
    );
}

void TcpLine::newConnection(){
    QTcpSocket *socket       = m_server->nextPendingConnection();
    TcpLineSocket* lineSocket = new TcpLineSocket(socket, this);
    connect(socket, &QTcpSocket::disconnected, [this, lineSocket](){ removeSocket(lineSocket); } );
    connect(socket, SIGNAL(disconnected()), lineSocket, SLOT(deleteLater()));
    connect(socket, &QTcpSocket::readyRead, [this, lineSocket](){ socketOutputReady(lineSocket); });
    m_sockets.append(lineSocket);

    vlog_debug("live-tcpline", "New connection: " + lineSocket->address());

    if ( !isAgentInitialized() ){
        initializeAgent();
    }
}

void TcpLine::socketOutputReady(TcpLineSocket *socket){
    if ( socket == agent() ){
        m_receivedBytes.append(socket->readAll());
        m_receivedOutput = MLNode(MLNode::Object);
        try{
            ml::fromJson(m_receivedBytes, m_receivedOutput);
            m_receivedBytes.clear();
            process();
        } catch ( lv::Exception& e ){
            ViewContext::instance().engine()->throwError(&e, this);
        }
    }
}

void TcpLine::startListening(){
    if ( !m_componentComplete )
        return;

    if ( m_server->isListening() ){
        m_server->close();
    }

    if ( m_address == "" ){
        if( !m_server->listen(QHostAddress::Any, m_port) ){
            lv::Exception e = CREATE_EXCEPTION(
                lv::Exception, "TcpLine: Cannot open tcp connection.", 0
            );
            lv::ViewContext::instance().engine()->throwError(&e);
            return;
        }
        m_address = m_server->serverAddress().toString();
        vlog_debug("live-tcpline", "Started listening on " + m_address + ":" + QString::number(m_port));

        emit listening();
    } else {
        if( !m_server->listen(QHostAddress(m_address), m_port) ){
            lv::Exception e = CREATE_EXCEPTION(
                lv::Exception, "TcpLine: Cannot open tcp connection to " + m_address.toStdString() + ".", 0
            );
            lv::ViewContext::instance().engine()->throwError(&e);
            return;
        }
        emit listening();
    }
}

void TcpLine::removeSocket(TcpLineSocket *socket){
    for ( auto it = m_sockets.begin(); it != m_sockets.end(); ++it ){
        if ( *it == socket ){
            m_sockets.erase(it);
            return;
        }
    }
}

void TcpLine::initializeAgent(){

    TcpLineSocket* a = agent();
    if ( !a || !m_componentComplete )
        return;

    if ( m_source && m_input && m_output ){

        SharedDataLocker* locker = createLocker();
        if ( m_input->reserveForRead(locker, this) && m_output->reserveForRead(locker, this)){

            m_initializer = MLNode(MLNode::Object);

            QString source = "Item" + m_source->sourceCode();
            m_initializer["source"] = source.toStdString();
            m_initializer["imports"] = m_source->importSourceCode().toStdString();

            MLNode inputResult;
            Tuple::serialize(ViewContext::instance().engine(), *m_input, inputResult);

            m_initializer["input"] = inputResult;

            MLNode outputResult;
            Tuple::serialize(ViewContext::instance().engine(), *m_output, outputResult);
            m_initializer["output"] = outputResult;

            vlog_debug("live-tcpline", "Initializing agent: " + a->address());

            a->initialize(m_initializer);

        }

        deleteLocker(locker);

    }
}

bool TcpLine::isAgentInitialized(){
    if ( !agent() )
        return false;
    return agent()->isInitialized();
}

TcpLineSocket *TcpLine::agent(){
    if ( m_sockets.isEmpty() )
        return 0;
    return m_sockets.first();
}

}// namespace

