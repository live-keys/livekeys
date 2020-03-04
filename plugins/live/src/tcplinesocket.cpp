#include "tcplinesocket.h"
#include "live/viewengine.h"
#include "live/viewcontext.h"
#include "live/exception.h"
#include "live/mlnode.h"
#include "live/mlnodetojson.h"
#include "live/metainfo.h"
#include "live/visuallogqt.h"

#include "remotelineresponse.h"
#include "tcplineserver.h"

#include <QTcpSocket>
#include <QHostAddress>
#include <QQmlComponent>
#include <QQmlContext>

namespace lv{

TcpLineSocket::TcpLineSocket(QTcpSocket *socket, QObject* parent)
    : QObject(parent)
    , m_socket(socket)
    , m_post(new QQmlPropertyMap)
    , m_response(new RemoteLineResponse(this))
    , m_component(new QQmlComponent())
    , m_componentContext(nullptr)
    , m_sourceItem(nullptr)
{
    m_response->onResponse([this](const QString& propertyName, const QVariant& value){
        responseValueChanged(propertyName, value);
    });

    connect(m_socket, &QTcpSocket::readyRead, this, &TcpLineSocket::tcpRead);
//    connect(m_socket, &QTcpSocket::error,     this, &TcpLineSocket::tcpError);
    m_socket->setSocketOption(QAbstractSocket::KeepAliveOption, true);
    m_address = m_socket->peerAddress().toString();

    m_lineCapture.onMessage(&TcpLineSocket::receiveMessage, this);
    m_lineCapture.onError([this](int, const std::string& errorString){
        lv::Exception e = CREATE_EXCEPTION(
            lv::Exception, "TcpLineSocket message capture error: " + errorString, 0
        );
        lv::ViewContext::instance().engine()->throwError(&e, this);
    });
}

TcpLineSocket::~TcpLineSocket(){
    if ( m_socket->state() == QTcpSocket::ConnectedState ){
        m_socket->disconnectFromHost();
        m_socket->waitForDisconnected(5000);
    }
    delete m_response;
    delete m_socket;
    delete m_post;
}

void TcpLineSocket::receiveMessage(const LineMessage &message, void *data){
    TcpLineSocket* tls = reinterpret_cast<TcpLineSocket*>(data);
    tls->onMessage(message);
}

void TcpLineSocket::onMessage(const LineMessage &message){
    if ( message.type & LineMessage::Build ){

        if ( m_sourceItem )
            delete m_sourceItem;

        m_componentContext = new QQmlContext(lv::ViewContext::instance().engine()->engine());
        m_componentContext->setContextProperty("post", QVariant::fromValue(m_post));
        m_componentContext->setContextProperty("response", QVariant::fromValue(m_response));

        m_component->setData(message.data, QUrl("Remote.qml"));

        m_sourceItem = m_component->create(m_componentContext);

    } else if ( message.type & LineMessage::Error ){
        if ( message.type & LineMessage::Json ){
            MLNode errorOb;
            ml::fromJson(message.data.data(), errorOb);
            server()->lineSocketError(
                this,
                QByteArray::fromStdString(errorOb["message"].asString()),
                errorOb["code"].asInt(),
                QString::fromStdString(errorOb["type"].asString())
            );
        } else {
            server()->lineSocketError(this, "Error", Exception::toCode("Remove"), message.data);
        }
    } else if ( message.type & LineMessage::Input ){

        if ( !m_component ){
            sendError("Error", Exception::toCode("~Build"), "Input sent without any build component.");
        }

        try{
            MLNode inputOb;
            ml::fromJson(message.data.data(), inputOb);

            ViewEngine* engine = ViewContext::instance().engine();

            for ( auto it = inputOb.begin(); it != inputOb.end(); ++it ){
                m_post->insert(
                    QByteArray::fromStdString(it.key().c_str()),
                    MetaInfo::deserializeVariant(engine, it.value())
                );
            }
        } catch ( Exception& e ){
            server()->lineSocketError(this, "Error", e.code(), QString::fromStdString(e.message()));
        }

    } else {
        server()->lineSocketError(this, "Error", Exception::toCode("Unknown"), "Unkown message type.");
    }
}

void TcpLineSocket::sendError(const QByteArray &type, Exception::Code code, const QString &message){

    MLNode errorObject = {
        {"code", MLNode((MLNode::IntType)code)},
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

void TcpLineSocket::responseValueChanged(const QString &key, const QVariant &value){
    MLNode n(MLNode::Object);

    MLNode result;
    MetaInfo::serializeVariant(ViewContext::instance().engine(), value, result);

    n[key.toStdString()] = result;

    std::string responseSerialized;
    ml::toJson(n, responseSerialized);

    QByteArray toSend = LineMessage::create(
        LineMessage::Input | LineMessage::Json,
        responseSerialized.c_str(),
        (int)responseSerialized.size()
    );
    m_socket->write(toSend);
}

void TcpLineSocket::tcpError(QAbstractSocket::SocketError){
    lv::Exception e = CREATE_EXCEPTION(
        lv::Exception, "Log listener socket error: " + m_socket->errorString().toStdString(), 0
    );
    lv::ViewContext::instance().engine()->throwError(&e, this);
}

void TcpLineSocket::tcpRead(){
    QByteArray ba = m_socket->readAll();
    m_lineCapture.append(ba);
}

TcpLineServer *TcpLineSocket::server(){
    return qobject_cast<TcpLineServer*>(parent());
}

}// namespace
