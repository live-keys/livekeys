#include "QCaptureContainer.hpp"

static QCamCaptureThread* QCaptureContainer::m_instance = 0;

QCaptureContainer::QCaptureContainer(QObject *parent) :
    QObject(parent)
{
}

QCamCaptureThread *QCaptureContainer::findCapture(int device){
    for( QList<QCamCaptureThread*>::iterator it = m_captureThreads.begin(); it != m_captureThreads.end(); ++it ){
        if ( (*it)->m_deviceId == device )
            return *it;
    }
    QCamCaptureThread* thread = new QCamCaptureThread();
    thread->m_deviceId = device;
    thread->capture    = new VideoCapture; // Let opening to the gui thread
    m_captureThreads.append(thread);
}


QCaptureContainer::~QCaptureContainer(){
    for( QList<QCamCaptureThread*>::iterator it = m_captureThreads.begin(); it != m_captureThreads.end(); ++it )
        delete *it;
    m_captureThreads.clear();
}
