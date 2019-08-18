#include "qmlfork.h"
#include "live/sharedmemoryreadworker.h"
#include "live/sharedmemorywriteworker.h"

#include "live/visuallogqt.h"
#include "live/mlnode.h"
#include "live/mlnodetojson.h"

#include <QDateTime>
#include <QProcess>

namespace lv{

QmlFork::QmlFork(QObject *parent)
    : RemoteContainer(parent)
    , m_handlerData(0)
    , m_fork(nullptr)
    , m_readSocket(nullptr)
    , m_writeSocket(nullptr)
    , m_forkready(false)
{
}

QmlFork::~QmlFork(){
    m_fork->close();
    if ( !m_fork->waitForFinished(1000) ){
        m_fork->kill();
    }

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

void QmlFork::sendError(const QByteArray &type, int code, const QString &message){
    MLNode errorObject = {
        {"code", code},
        {"message", message.toStdString()},
        {"type", type.toStdString()}
    };

    std::string errorSerialized;
    ml::toJson(errorObject, errorSerialized);

    m_writeSocket->write(errorSerialized.c_str(), LineMessage::Error | LineMessage::Json);
}

void QmlFork::sendBuild(const QByteArray &buildData){
    m_writeSocket->write(buildData, LineMessage::Build | LineMessage::Raw);
    m_writeSocket->write(buildData, LineMessage::Build | LineMessage::Raw);
}

void QmlFork::sendInput(const MLNode &input){
    std::string inputSerialized;
    ml::toJson(input, inputSerialized);

    m_writeSocket->write(inputSerialized.c_str(), LineMessage::Input | LineMessage::Json);
}

void QmlFork::onMessage(std::function<void (const LineMessage &, void *)> handler, void *handlerData){
    m_handler = handler;
    m_handlerData = handlerData;
}

void QmlFork::onError(std::function<void (int, const std::string &)> handler){
    m_errorHandler = handler;
}

void QmlFork::onSharedMemoryReadStatusChanged(int status){
    if ( status == SharedMemoryReadWorker::Initializing ){
        vlog("shared-memory").v() << "Fork: Initializing shared memory for writing \'" << m_writeSocket->key() << "\'.";
    } else if ( status == SharedMemoryReadWorker::Attached ){
        vlog("shared-memory").v() << "Fork: Attached shared memory for writing at \'" << m_writeSocket->key() << "\'.";
    } else if ( status == SharedMemoryReadWorker::Created ){
        vlog("shared-memory").v() << "Fork: Created shared memory for writing \'" << m_writeSocket->key() << "\'.";
    }
}

void QmlFork::onSharedMemoryMessage(const QByteArray &message, int type, int id){
    if ( type == LineMessage::Json && message == "\"ready\""){
        if ( !m_forkready ){
            m_forkready = true;
            emit ready();
        }
    } else {
        LineMessage lm;
        lm.data = message;
        lm.type = type;
        lm.id   = id;
        m_handler(lm, m_handlerData);
    }
}

void QmlFork::onSharedMemoryWriteStatusChanged(int status){
    if ( status == SharedMemoryWriteWorker::Initializing ){
        vlog("shared-memory").v() << "Fork: Initializing shared memory for reading \'" << m_readSocket->key() << "\'.";
    } else if ( status == SharedMemoryWriteWorker::Attached ){
        vlog("shared-memory").v() << "Fork: Attached shared memory for reading \'" << m_readSocket->key() << "\'.";
    } else if ( status == SharedMemoryWriteWorker::Created ){
        vlog("shared-memory").v() << "Fork: Created shared memory for reading \'" << m_readSocket->key() << "\'.";
    }
}

void QmlFork::componentComplete(){
    RemoteContainer::componentComplete();

    QDateTime cdt = QDateTime::currentDateTime();
    QString forkId = cdt.toString("yyyyMMddhhmmsszzz");

    QString program = "./livecv";

    m_fork = new QProcess(this);

    QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
    env.insert("lk-fork-id", forkId);
    m_fork->setProcessEnvironment(env);

    QStringList arguments;
    if ( !m_layers.isEmpty() ){
        arguments.append("--layers");
        arguments.append(m_layers);
    }
    arguments.append("plugins/live/forknode.qml");

    m_readSocket = new SharedMemoryReadWorker(forkId + "-fp", this);
    m_writeSocket = new SharedMemoryWriteWorker(forkId + "-pf");

    connect(m_readSocket, &SharedMemoryReadWorker::statusUpdate, this, &QmlFork::onSharedMemoryReadStatusChanged);
    connect(m_readSocket, &SharedMemoryReadWorker::message, this, &QmlFork::onSharedMemoryMessage);
    connect(m_writeSocket, &SharedMemoryWriteWorker::statusUpdate, this, &QmlFork::onSharedMemoryWriteStatusChanged);

    m_writeSocket->start();
    m_readSocket->start();

    m_fork->start(program, arguments);
}

}// namespace

