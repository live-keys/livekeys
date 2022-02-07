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

#include "qmlforknode.h"
#include "live/sharedmemoryreadworker.h"
#include "live/sharedmemorywriteworker.h"

#include "live/linemessage.h"
#include "live/visuallogqt.h"
#include "live/exception.h"
#include "live/viewengine.h"
#include "live/viewcontext.h"
#include "live/mlnode.h"
#include "live/mlnodetojson.h"

#include "remotelineresponse.h"

#include <QDebug>
#include <QQmlComponent>
#include <QQmlContext>

namespace lv{

QmlForkNode::QmlForkNode(QObject *parent)
    : QObject(parent)
    , m_readSocket(nullptr)
    , m_writeSocket(nullptr)
    , m_post(new QQmlPropertyMap)
    , m_response(new RemoteLineResponse(this))
    , m_component()
    , m_componentContext(nullptr)
    , m_sourceItem(nullptr)
{
    m_response->onResponse([this](const QString& propertyName, const QVariant& value){
        responseValueChanged(propertyName, value);
    });
}

QmlForkNode::~QmlForkNode(){
    delete m_response;
    delete m_post;

    m_readSocket->exit();
    if(!m_readSocket->wait(1000)){
      m_readSocket->terminate();
      m_readSocket->wait(ULONG_MAX);
    }
    m_writeSocket->exit();
    if(!m_writeSocket->wait(1000)){
        m_writeSocket->terminate();
        m_writeSocket->wait(ULONG_MAX);
    }
}

void QmlForkNode::responseValueChanged(const QString &key, const QVariant &value){
    MLNode n(MLNode::Object);

    MLNode result;
    MetaInfo::serializeVariant(ViewContext::instance().engine(), value, result);

    n[key.toStdString()] = result;

    std::string responseSerialized;
    ml::toJson(n, responseSerialized);

    m_writeSocket->write(QByteArray(responseSerialized.c_str(), (int)responseSerialized.size()), LineMessage::Input | LineMessage::Json);
}

void QmlForkNode::sendError(const QByteArray &type, Exception::Code code, const QString &message){
    MLNode errorObject = {
        {"code", MLNode((MLNode::IntType)code)},
        {"message", message.toStdString()},
        {"type", type.toStdString()}
    };

    std::string errorSerialized;
    ml::toJson(errorObject, errorSerialized);

    m_writeSocket->write(QByteArray(errorSerialized.c_str(), (int)errorSerialized.size()), LineMessage::Error | LineMessage::Json);
}

void QmlForkNode::componentComplete(){
    QByteArray sharedMemoryKey = qgetenv("lk-fork-id");
    if (sharedMemoryKey.isEmpty()){
        Exception e = CREATE_EXCEPTION(Exception, "Failed to find shared memory key in environment.", Exception::toCode("~Key"));
        ViewContext::instance().engine()->throwError(&e, this);
        return;
    }

    m_readSocket = new SharedMemoryReadWorker(sharedMemoryKey + "-pf", this);
    m_writeSocket = new SharedMemoryWriteWorker(sharedMemoryKey + "-fp");

    connect(m_readSocket,  &SharedMemoryReadWorker::statusUpdate,  this, &QmlForkNode::onSharedMemoryReadStatusChanged);
    connect(m_readSocket,  &SharedMemoryReadWorker::message, this, &QmlForkNode::onSharedMemoryMessage);
    connect(m_writeSocket, &SharedMemoryWriteWorker::statusUpdate, this, &QmlForkNode::onSharedMemoryWriteStatusChanged);

    m_writeSocket->start();
    m_readSocket->start();
}

void QmlForkNode::onSharedMemoryReadStatusChanged(int status){
    if ( status == SharedMemoryReadWorker::Initializing ){
        vlog("shared-memory").v() << "ForkNode: Initializing shared memory for reading.";
    } else if ( status == SharedMemoryReadWorker::Attached ){
        vlog("shared-memory").v() << "ForkNode: Attached shared memory for reading.";
    } else if ( status == SharedMemoryReadWorker::Created ){
        vlog("shared-memory").v() << "ForkNode: Created shared memory for reading.";
    }

    if ( m_readSocket->isReady() && m_writeSocket->isReady() ){
        m_writeSocket->write("\"ready\"", LineMessage::Json);
    }
}

void QmlForkNode::onSharedMemoryMessage(const QByteArray &message, int type, int){
    if ( type & LineMessage::Build ){

        if ( m_sourceItem )
            delete m_sourceItem;

        m_componentContext = new QQmlContext(lv::ViewContext::instance().engine()->engine());
        m_componentContext->setContextProperty("post", QVariant::fromValue(m_post));
        m_componentContext->setContextProperty("response", QVariant::fromValue(m_response));

        m_component = new QQmlComponent(ViewContext::instance().engine()->engine());
        m_component->setData(message, QUrl("Remote.qml"));

        m_sourceItem = m_component->create(m_componentContext);
    } else if ( type & LineMessage::Error ){
        if ( type & LineMessage::Json ){
            MLNode errorOb;
            ml::fromJson(message.data(), errorOb);
            emit error(
                QByteArray::fromStdString(errorOb["message"].asString()),
                errorOb["code"].asInt(),
                QString::fromStdString(errorOb["type"].asString())
            );
        } else {
            emit error("Error", Exception::toCode("Remove"), message);
        }
    } else if ( type & LineMessage::Input ){

        if ( !m_component ){
            sendError("Error", Exception::toCode("~Build"), "Input sent without any build component.");
        }

        try{
            MLNode inputOb;
            ml::fromJson(message.data(), inputOb);

            ViewEngine* engine = ViewContext::instance().engine();

            for ( auto it = inputOb.begin(); it != inputOb.end(); ++it ){
                m_post->insert(
                    QByteArray::fromStdString(it.key().c_str()),
                    MetaInfo::deserializeVariant(engine, it.value())
                );
            }
        } catch ( Exception& e ){
            emit error("Error", e.code(), QString::fromStdString(e.message()));
        }

    } else {
        emit error("Error", Exception::toCode("Unknown"), "Unkown message type.");
    }
}

void QmlForkNode::onSharedMemoryWriteStatusChanged(int status){
    if ( status == SharedMemoryWriteWorker::Initializing ){
        vlog("shared-memory").v() << "ForkNode: Initializing shared memory for writing.";
    } else if ( status == SharedMemoryWriteWorker::Attached ){
        vlog("shared-memory").v() << "ForkNode: Attached shared memory for writing.";
    } else if ( status == SharedMemoryWriteWorker::Created ){
        vlog("shared-memory").v() << "ForkNode: Created shared memory for writing.";
    }

    if ( m_readSocket->isReady() && m_writeSocket->isReady() ){
        m_writeSocket->write("\"ready\"", LineMessage::Json);
    }
}

}// namespace
