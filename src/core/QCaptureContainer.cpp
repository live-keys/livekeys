#include "QCaptureContainer.hpp"
#include "QCamCaptureThread.hpp"
#include "QVideoCaptureThread.hpp"

QCaptureContainer* QCaptureContainer::m_instance = 0;

QCaptureContainer::QCaptureContainer(QObject *parent) :
    QObject(parent)
{
}

QCamCaptureThread *QCaptureContainer::captureThread(int device){
    for( QList<QCamCaptureThread*>::iterator it = m_camCaptureThreads.begin(); it != m_camCaptureThreads.end(); ++it ){
        if ( (*it)->device() == device )
            return *it;
    }
    QCamCaptureThread* thread = new QCamCaptureThread(device);
    m_camCaptureThreads.append(thread);
    return thread;
}

QVideoCaptureThread *QCaptureContainer::captureThread(const QString &file)
{
    for( QList<QVideoCaptureThread*>::iterator it = m_videoCaptureThreads.begin(); it != m_videoCaptureThreads.end(); ++it ){
        if ( (*it)->file() == file )
            return *it;
    }
    QVideoCaptureThread* thread = new QVideoCaptureThread(file);
    m_videoCaptureThreads.append(thread);
    return thread;
}


QCaptureContainer::~QCaptureContainer(){
    for( QList<QCamCaptureThread*>::iterator it = m_camCaptureThreads.begin(); it != m_camCaptureThreads.end(); ++it )
        delete *it;
    m_camCaptureThreads.clear();
}
