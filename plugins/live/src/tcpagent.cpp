#include "tcpagent.h"
#include "live/mlnode.h"
#include "live/mlnodetojson.h"
#include "live/plugincontext.h"
#include "live/engine.h"
#include "live/visuallog.h"
#include "tuplepropertymap.h"
#include <QQmlComponent>
#include <QQmlContext>
#include <QQmlEngine>
#include <QTcpSocket>
#include <QTimer>

namespace lv{

int TcpAgent::RECONNECT_TIMEOUT = 5000;

TcpAgent::TcpAgent(QObject *parent)
    : QObject(parent)
    , m_socket(new QTcpSocket(this))
    , m_port(-1)
    , m_timer(0)
    , m_component(0)
    , m_componentContext(0)
    , m_componentComplete(false)
    , m_root(0)
    , m_input(0)
    , m_output(0)
    , m_sourceItem(0)
{
    connect(m_socket, SIGNAL(connected()),    this, SLOT(socketConnected()));
    connect(m_socket, SIGNAL(disconnected()), this, SLOT(socketDisconnected()));
    connect(m_socket, SIGNAL(readyRead()),    this, SLOT(socketData()));
    connect(m_socket, SIGNAL(error(QAbstractSocket::SocketError)),
            this, SLOT(socketError(QAbstractSocket::SocketError)));
}

TcpAgent::~TcpAgent(){
    delete m_timer;
    if ( m_socket->state() == QTcpSocket::ConnectedState ){
        m_socket->disconnectFromHost();
        m_socket->waitForDisconnected(5000);
    }
}

void TcpAgent::connectToHost(){
    if ( m_componentComplete && m_address != "" && m_port != -1 ){
        reconnect();
    }
}

void TcpAgent::sendError(const QByteArray& type, int code, const QString &message){
    MLNode errorObject = {
        {"error", {
             {"code", code},
             {"message", message.toStdString()},
             {"type", type.toStdString()}
         }}
    };
    QByteArray errorSerialized;
    ml::toJson(errorObject, errorSerialized);

    m_socket->write(errorSerialized);
}

void TcpAgent::componentComplete(){
    m_componentComplete = true;
    connectToHost();
}

void TcpAgent::socketConnected(){
    vlog_debug("live-tcpagent", "Connected to :" + m_address);
}

void TcpAgent::socketDisconnected(){
    qWarning(
        "Disconnected from %s: \'%s\'. Retrying...",
        qPrintable(m_address),
        qPrintable(m_socket->errorString())
    );
    timer()->start(RECONNECT_TIMEOUT);
}

void TcpAgent::reconnect(){
    m_socket->connectToHost(m_address, m_port, QTcpSocket::ReadWrite);
}

void TcpAgent::socketData(){
    m_collectedData.append(m_socket->readAll());

    try{
        MLNode n;
        ml::fromJson(m_collectedData, n);
        m_collectedData.clear();

        if ( n.hasKey("source") ){

            delete m_component;
            delete m_sourceItem;
            delete m_componentContext;
            delete m_root;

            m_root             = 0;
            m_input            = 0;
            m_output           = 0;
            m_sourceItem       = 0;
            m_componentContext = 0;

            QObject* importsObject = 0;
            if ( n.hasKey("imports" ) ){
                m_component = new QQmlComponent(qmlEngine(this), this);
                QByteArray data = QByteArray::fromStdString(n["imports"].asString()) + "\nQtObject{}";
                m_component->setData(data, QUrl());
                importsObject = m_component->create();
            }

            if ( n.hasKey("input") ){
                m_input = new QQmlPropertyMap(m_root);
                TuplePropertyMap::deserialize(PluginContext::engine(), n["input"], *m_input);
            }
            if ( n.hasKey("output") ){
                m_output = new QQmlPropertyMap(m_root);
                TuplePropertyMap::deserialize(PluginContext::engine(), n["output"], *m_output);
            }

            if ( importsObject ){
                delete importsObject;
                delete m_component;
                m_component = 0;
            }

            QQmlContext* ctx = qmlContext(this);

            m_root = new QQmlPropertyMap;

            if ( m_input )
                m_root->insert("input", QVariant::fromValue(m_input));
            if ( m_output ){
                m_root->insert("output", QVariant::fromValue(m_output));
                connect(m_output, &QQmlPropertyMap::valueChanged, this, &TcpAgent::outputValueChanged);
            }

            m_componentContext = new QQmlContext(ctx);
            m_componentContext->setContextProperty("tcp", QVariant::fromValue(m_root));

            m_component = new QQmlComponent(qmlEngine(this), this);
            m_component->setData(
                QByteArray::fromStdString(n["imports"].asString() + n["source"].asString()), ctx->baseUrl()
            );
            m_sourceItem = m_component->create(m_componentContext);

        } else if ( !m_component ){
            sendError("Error", 0, "Data sent before component initialization.");

            //TODO Reserialize input
        }

    } catch ( lv::Exception&){
    }
}

void TcpAgent::outputValueChanged(const QString &, const QVariant &){
    MLNode n(MLNode::Object);
    MLNode result;

    TuplePropertyMap::serialize(lv::PluginContext::engine(), *m_output, result);
    n["output"] = result;

    QByteArray bytes;
    ml::toJson(n, bytes);

    m_socket->write(bytes);
}

void TcpAgent::socketError(QAbstractSocket::SocketError){
    m_socket->abort();
    qWarning(
        "Failed to connect to %s: \'%s\'. Reconnecting...",
        qPrintable(m_address),
        qPrintable(m_socket->errorString())
    );
    timer()->start(RECONNECT_TIMEOUT);
}

QTimer *TcpAgent::timer(){
    if ( !m_timer ){
        m_timer = new QTimer;
        m_timer->setSingleShot(true);
        connect(m_timer, SIGNAL(timeout()), this, SLOT(reconnect()));
    }

    return m_timer;
}
}// namespace

