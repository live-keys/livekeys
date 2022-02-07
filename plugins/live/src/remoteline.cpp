/****************************************************************************
**
** Copyright (C) 2022 Dinu SV.
** This file is part of Livekeys Application.
**
** GNU Lesser General Public License Usage
** This file may be used under the terms of the GNU Lesser
** General Public License version 3 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPLv3 included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 3 requirements
** will be met: https://www.gnu.org/licenses/lgpl.html.
**
****************************************************************************/

#include "remoteline.h"
#include "remotelineproperty.h"

#include "live/metainfo.h"
#include "live/visuallogqt.h"
#include "live/viewcontext.h"
#include "live/viewengine.h"
#include "live/mlnode.h"
#include "live/mlnodetojson.h"

#include <QQmlProperty>

namespace lv{

RemoteLine::RemoteLine(QObject *parent)
    : QObject(parent)
    , m_componentComplete(false)
    , m_componentBuild(false)
    , m_source(nullptr)
    , m_connection(nullptr)
    , m_result(new QQmlPropertyMap)
{
}

RemoteLine::~RemoteLine(){
    delete m_result;
}

void RemoteLine::propertyChanged(RemoteLineProperty *property){
    if ( m_componentBuild ){
        sendProperty(property->name());
    } else {
        m_propertiesToSend.insert(property->name());
    }
}

void RemoteLine::sendProperty(const QString &propertyName){
    MLNode input = MLNode(MLNode::Object);
    MLNode inputValue;

    QQmlProperty pp(this, propertyName);

    MetaInfo::serializeVariant(lv::ViewContext::instance().engine(), pp.read(), inputValue);

    input[propertyName.toStdString()] = inputValue;

    vlog("remote-line").v() << "Sending property to remote: " << propertyName;

    m_connection->sendInput(input);
}

void RemoteLine::componentComplete(){
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

                vlog("remote-line").v() << "Monitoring property; " << property.name();

                RemoteLineProperty* tlp = new RemoteLineProperty(property.name(), this);
                m_properties.append(tlp);
                pp.connectNotifySignal(tlp, SLOT(changed()));
            }
        }
    }

    emit complete();
}

void RemoteLine::setConnection(RemoteContainer *connection){
    if (m_connection == connection)
        return;

    m_connection = connection;
    emit connectionChanged();

    m_connection->onMessage(&RemoteLine::receiveMessage, this);
    m_connection->onError([this](int, const std::string& errorString){
        lv::Exception e = CREATE_EXCEPTION(
            lv::Exception, "TcpLine message capture error: " + errorString, 0
        );
        lv::ViewContext::instance().engine()->throwError(&e, this);
    });

    connect(m_connection, &RemoteContainer::ready, this, &RemoteLine::initialize);

    initialize();
}

void RemoteLine::receiveMessage(const LineMessage &message, void *data){
    RemoteLine* tls = reinterpret_cast<RemoteLine*>(data);
    tls->onMessage(message);
}

void RemoteLine::onMessage(const LineMessage &message){
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
                    MetaInfo::deserializeVariant(engine, it.value())
                );
            }

            emit resultChanged();

        } catch ( Exception& e ){
            lv::ViewContext::instance().engine()->throwError(&e, this);
            return;
        }

    }
}

void RemoteLine::initialize(){

    if ( m_source && m_connection && m_connection->isReady() ){

        QByteArray source =
            m_source->importSourceCode().toUtf8() +
            "\nItem" +
            m_source->sourceCode().toUtf8();

        vlog("remote-line").v() << "Initializing remote component.";

        m_connection->sendBuild(source);

        m_componentBuild = true;

        for ( auto it = m_properties.begin(); it != m_properties.end(); ++it ){
            RemoteLineProperty* tlp = *it;
            sendProperty(tlp->name());
        }
    }
}

void RemoteLine::setSource(ComponentSource *source){
    if (m_source == source)
        return;

    m_source = source;
    emit sourceChanged();

    initialize();
}

}// namespace
