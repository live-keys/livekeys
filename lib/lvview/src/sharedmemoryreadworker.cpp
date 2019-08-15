#include "sharedmemoryreadworker.h"
#include "sharedmemorywriteworker.h"

#include "live/linecapture.h"
#include "live/visuallogqt.h"
#include "live/exception.h"

#include <QDebug>

namespace lv{

SharedMemoryReadWorker::SharedMemoryReadWorker(const QString &sharedMemoryKey, QObject *parent)
    : QThread(parent)
    , m_memory(sharedMemoryKey, this)
    , m_isReady(false)
{
    m_lineCapture.onMessage(&SharedMemoryReadWorker::receiveMessage, this);
    m_lineCapture.onError([this](int code, const std::string& errorString){
        emit error(code, QString::fromStdString(errorString));
    });
}

SharedMemoryReadWorker::~SharedMemoryReadWorker(){
}

void SharedMemoryReadWorker::receiveMessage(const LineMessage &message, void *data){
    vlog() << "MESSAGE RECEIVED of size:" << message.data.size();
    SharedMemoryReadWorker* s = reinterpret_cast<SharedMemoryReadWorker*>(data);
    s->onMessage(message);
}

void SharedMemoryReadWorker::run(){
    emit statusUpdate(SharedMemoryReadWorker::Initializing);

    if ( !m_memory.create(SharedMemoryWriteWorker::SHARED_MEMORY_SIZE) ){
        if ( !m_memory.attach(QSharedMemory::ReadWrite) ){
            emit error(Exception::toCode("~Shared"), "Failed to create memory at key: " + m_memory.key());
            return;
        }
        m_isReady = true;
        emit statusUpdate(SharedMemoryReadWorker::Attached);
    } else {
        m_isReady = true;
        emit statusUpdate(SharedMemoryReadWorker::Created);
    }

    vlog() << "MEMORY LOCK:" << m_memory.lock();
    readMemory();
    vlog() << "MEMORY UNLOCK:" << m_memory.unlock();

    forever{
        m_memory.lock();
        readMemory();
        m_memory.unlock();
    }
}

void SharedMemoryReadWorker::onMessage(const LineMessage &msg){
    vlog() << "ON MESSAGE RECEIVED WITH LINE MESSAGE";
    emit message(msg.data, msg.type, msg.id);
}

void SharedMemoryReadWorker::readMemory(){
    char* data = reinterpret_cast<char*>(m_memory.data());
    vlog() << "REE:" << data;
//    QByteArray ba = QByteArray::fromRawData(data, m_memory.size());

    m_lineCapture.append(QByteArray::fromRawData(data, m_memory.size()));
//    vlog() << "READ:" << ba.size();
//    vlog() << "READ:" << ba;
//    m_lineCapture.append(ba);

    memset(data, 0, m_memory.size());
}

} // namespace
