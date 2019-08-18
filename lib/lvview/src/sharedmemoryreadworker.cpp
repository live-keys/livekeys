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

    readMemory();

    forever{
        m_memory.lock();
        readMemory();
        m_memory.unlock();
    }
}

void SharedMemoryReadWorker::onMessage(const LineMessage &msg){
    emit message(msg.data, msg.type, msg.id);
}

void SharedMemoryReadWorker::readMemory(){
    char* data = reinterpret_cast<char*>(m_memory.data());

    m_lineCapture.append(QByteArray::fromRawData(data, m_memory.size()));

    memset(data, 0, m_memory.size());
}

} // namespace
