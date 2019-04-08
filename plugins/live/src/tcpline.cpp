#include "tcpline.h"
#include "tcplineproperty.h"

#include "live/typeinfo.h"
#include "live/visuallogqt.h"
#include "live/viewcontext.h"
#include "live/viewengine.h"
#include "live/mlnode.h"
#include "live/mlnodetojson.h"

#include <QQmlProperty>

namespace lv{

TcpLine::TcpLine(QObject *parent)
    : QObject(parent)
    , m_componentComplete(false)
    , m_componentBuild(false)
    , m_source(nullptr)
    , m_connection(nullptr)
    , m_result(new QQmlPropertyMap)
{
}

TcpLine::~TcpLine(){
    delete m_result;
}

void TcpLine::propertyChanged(TcpLineProperty *property){
    if ( m_componentBuild ){
        sendProperty(property->name());
    } else {
        m_propertiesToSend.insert(property->name());
    }
}

void TcpLine::sendProperty(const QString &propertyName){
    MLNode input = MLNode(MLNode::Object);
    MLNode inputValue;

    QQmlProperty pp(this, propertyName);

    TypeInfo::serializeVariant(lv::ViewContext::instance().engine(), pp.read(), inputValue);

    input[propertyName.toStdString()] = inputValue;

    vlog("tcp-line").d() << "Sending property to remote: " << propertyName;

    m_connection->sendInput(input);
}

void TcpLine::componentComplete(){
    m_componentComplete = true;

    const QMetaObject *meta = metaObject();
    for (int i = 0; i < meta->propertyCount(); i++){
        QMetaProperty property = meta->property(i);
        if ( property.name() != QByteArray("objectName") &&
             property.name() != QByteArray("source") &&
             property.name() != QByteArray("connection") &&
             property.name() != QByteArray("result")
        ){
            QQmlProperty pp(this, property.name());
            if ( pp.hasNotifySignal() ){

                vlog("tcp-line").d() << "Monitoring property; " << property.name();

                TcpLineProperty* tlp = new TcpLineProperty(property.name(), this);
                m_properties.append(tlp);
                pp.connectNotifySignal(tlp, SLOT(changed()));
            }
        }
    }

    emit complete();
}

void TcpLine::setConnection(TcpLineConnection *connection){
    if (m_connection == connection)
        return;

    m_connection = connection;
    emit connectionChanged();

    m_connection->dataCapture().onMessage(&TcpLine::receiveMessage, this);
    m_connection->dataCapture().onError([this](int, const std::string& errorString){
        lv::Exception e = CREATE_EXCEPTION(
            lv::Exception, "TcpLine message capture error: " + errorString, 0
        );
        lv::ViewContext::instance().engine()->throwError(&e, this);
    });

    connect(m_connection, SIGNAL(connectionEstablished()), this, SLOT(initialize()));

    initialize();
}

void TcpLine::receiveMessage(const LineMessage &message, void *data){
    TcpLine* tls = reinterpret_cast<TcpLine*>(data);
    tls->onMessage(message);
}

void TcpLine::onMessage(const LineMessage &message){
    if ( message.type & LineMessage::Error ){
        lv::Exception e = CREATE_EXCEPTION(
            lv::Exception, "TcpLine error: " + std::string(message.data), 0
        );
        lv::ViewContext::instance().engine()->throwError(&e, this);
    } else if ( message.type & LineMessage::Input ){

        try{
            MLNode inputOb;
            ml::fromJson(message.data.data(), inputOb);

            ViewEngine* engine = ViewContext::instance().engine();

            for ( auto it = inputOb.begin(); it != inputOb.end(); ++it ){
                m_result->insert(
                    QByteArray::fromStdString(it.key().c_str()),
                    TypeInfo::deserializeVariant(engine, it.value())
                );
            }

            emit resultChanged();

        } catch ( Exception& e ){
            lv::ViewContext::instance().engine()->throwError(&e, this);
            return;
        }

    }
}

void TcpLine::initialize(){

    if ( m_source && m_connection && m_connection->isConnected() ){

        QByteArray source =
            m_source->importSourceCode().toUtf8() +
            "\nItem" +
            m_source->sourceCode().toUtf8();

        vlog("tcp-line").d() << "Initializing remote component.";

        m_connection->sendBuild(source);

        m_componentBuild = true;

        for ( auto it = m_properties.begin(); it != m_properties.end(); ++it ){
            TcpLineProperty* tlp = *it;
            sendProperty(tlp->name());
        }
    }
}

void TcpLine::setSource(ComponentSource *source){
    if (m_source == source)
        return;

    m_source = source;
    emit sourceChanged();

    initialize();
}

}// namespace
