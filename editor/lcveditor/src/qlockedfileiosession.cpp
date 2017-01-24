#include "qlockedfileiosession.h"
#include <QFile>
#include <QTextStream>
#include <QMutex>
#include <QReadWriteLock>

namespace lcv{

class QFileLock{
public:
    QFileLock() : refcount(0){}

    QReadWriteLock lock;
    int refcount;
};

QLockedFileIOSession::QLockedFileIOSession()
    : m_locksMutex(new QMutex)
{
}

QReadWriteLock *QLockedFileIOSession::getLock(const QString &path){
    m_locksMutex->lock();
    QFileLock* fl = m_locks.value(path, 0);
    if ( !fl ){
        fl = new QFileLock;
        m_locks.insert(path, fl);
    }
    fl->refcount++;

    m_locksMutex->unlock();
    return &fl->lock;
}

void QLockedFileIOSession::releaseLock(const QString &path){
    m_locksMutex->lock();
    QFileLock* fl = m_locks.value(path, 0);
    if ( fl ){
        fl->lock.unlock();
        if ( --(fl->refcount) <= 0 )
            delete m_locks.take(path);
    }
    m_locksMutex->unlock();
}

QLockedFileIOSession::~QLockedFileIOSession(){
    delete m_locksMutex;
}

QLockedFileIOSession::Ptr QLockedFileIOSession::createInstance(){
    return QLockedFileIOSession::Ptr(new QLockedFileIOSession);
}

QString QLockedFileIOSession::readFromFile(const QString &path){
    getLock(path)->lockForRead();

    QFile fileInput(path);
    if ( !fileInput.open(QIODevice::ReadOnly ) ){
        qCritical("Cannot open file: %s", qPrintable(path));
        releaseLock(path);
        return "";
    }

    QTextStream in(&fileInput);
    QString content = in.readAll();
    fileInput.close();

    releaseLock(path);
    return content;
}

bool QLockedFileIOSession::writeToFile(const QString &path, const QString &data){
    getLock(path)->lockForWrite();

    QFile fileInput(path);
    if ( !fileInput.open(QIODevice::WriteOnly ) ){
        qCritical("Can't open file for writing: %s", qPrintable(path));
        releaseLock(path);
        return false;
    }

    QTextStream stream(&fileInput);
    stream << data;
    stream.flush();
    fileInput.close();

    releaseLock(path);
    return true;
}

}// namespace
