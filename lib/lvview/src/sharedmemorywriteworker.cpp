#include "sharedmemorywriteworker.h"

#include "live/linecapture.h"
#include "live/visuallogqt.h"
#include "live/viewcontext.h"
#include "live/exception.h"
#include "live/viewengine.h"

#include <QThread>
#include <QDebug>

namespace lv{

SharedMemoryWriteWorker::SharedMemoryWriteWorker(const QString& sharedMemoryKey, QObject *parent)
    : QObject(parent)
    , m_isReady(false)
    , m_memory(sharedMemoryKey)
    , m_thread(new QThread)
{
    moveToThread(m_thread);
    connect(m_thread, &QThread::started, this, &SharedMemoryWriteWorker::onInitialize);
    connect(m_thread, &QThread::finished, this, &SharedMemoryWriteWorker::onFinish);
    connect(this, &SharedMemoryWriteWorker::requestWrite, this, &SharedMemoryWriteWorker::onRequestWrite);
}

SharedMemoryWriteWorker::~SharedMemoryWriteWorker(){
    m_thread->exit();
    m_thread->wait();
    delete m_thread;
}

void SharedMemoryWriteWorker::start(){
    m_thread->start();
}

void SharedMemoryWriteWorker::exit(){
    m_thread->exit();
}

void SharedMemoryWriteWorker::terminate(){
    m_thread->terminate();
}

bool SharedMemoryWriteWorker::wait(unsigned long ms){
    return m_thread->wait(ms);
}

void SharedMemoryWriteWorker::write(const QByteArray &message, int type, int id){
    emit requestWrite(message, type, id);
}

void SharedMemoryWriteWorker::onInitialize(){
    emit statusUpdate(SharedMemoryWriteWorker::Initializing);
    if ( !m_memory.create(SharedMemoryWriteWorker::SHARED_MEMORY_SIZE) ){
        if ( !m_memory.attach() ){
            emit error(Exception::toCode("~Shared"), "Failed to create memory at key: " + m_memory.key());
            return;
        }
        m_memory.lock();
        m_isReady = true;
        emit statusUpdate(SharedMemoryWriteWorker::Attached);
    } else {
        m_memory.lock();
        char* data = reinterpret_cast<char*>(m_memory.data());
        memset(data, 0, m_memory.size());
        m_memory.unlock();
        m_isReady = true;
        emit statusUpdate(SharedMemoryWriteWorker::Created);
    }

}

void SharedMemoryWriteWorker::onFinish(){
    m_memory.unlock();
}

void SharedMemoryWriteWorker::onRequestWrite(QByteArray message, int type, int id){
    QByteArray from = LineMessage::create(type, message, id);
    char *to        = (char*)m_memory.data();

    //TODO: if from is bigger than allocated memory size
    memcpy(to, from.data(), from.length());

    forever{
        m_memory.unlock();
        m_memory.lock(); // lock semaphore back

        char c = ((char*)m_memory.data())[0];

        if ( c == 0 )
            break;
    }
}

}// namespace
