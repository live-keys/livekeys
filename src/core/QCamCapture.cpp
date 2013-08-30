#include "QCamCapture.hpp"

int i = 0;

VideoCapture* vcapture = 0;

QCamCaptureThread::QCamCaptureThread(QObject *parent):QThread(parent){
    if ( vcapture == 0 )
        vcapture = new VideoCapture;
    capture = vcapture;
}

QCamCaptureThread::~QCamCaptureThread(){
    m_mutex.lock();
    m_abord = true;
    m_condition.wakeOne();
    m_mutex.unlock();
    delete inactiveMat;
    wait(); // wait before base class destructor is invoked
}

void QCamCaptureThread::tick(){
    QMutexLocker lock(&m_mutex);
    if ( !isRunning() ){
        start(LowPriority);
    } else {
        inactiveMatReady = false;
        m_condition.wakeOne();
    }
}

void QCamCaptureThread::run(){
    forever{
        if ( capture->grab() ){

            capture->retrieve(*inactiveMat->data());
            inactiveMatReady = true;
            //QString name = QString("D:/Cam/") + QString::number(i) + QString(".jpg");
            //cv::imwrite(name.toStdString(), *inactiveMat->data());
            //qDebug() << capture->isOpened();
            //qDebug() << "mat received";
            emit inactiveMatChanged();
        } else
            qDebug() << "Open CV : No image captured";

        m_mutex.lock();
        if ( inactiveMatReady )
            m_condition.wait(&m_mutex);
        inactiveMatReady = true;
        if ( m_abord ){
            m_mutex.unlock();
            return;
        }
    }
}


QCamCapture::QCamCapture(QQuickItem *parent) :
    QMatDisplay(parent),
    m_device(-1),
    m_fps(25),
    m_paused(false),
    d_ptr(new QCamCaptureThread)
{
    d_ptr->inactiveMat      = new QMat();
    d_ptr->inactiveMatReady = false;
    connect(&d_ptr->timer, SIGNAL(timeout()),d_ptr, SLOT(tick()));
    connect( d_ptr, SIGNAL(inactiveMatChanged()), this, SLOT(switchMat()));
}

void QCamCapture::setDevice(int device){
    if ( m_device != device ){
        m_device = device;
        if ( m_device  != -1 ){
            if ( !d_ptr->capture->isOpened() )
                d_ptr->capture->open(m_device);

            if ( d_ptr->capture->isOpened() ){

                if ( !m_paused ){
                    if ( d_ptr->timer.isActive() )
                        d_ptr->timer.stop();
                    d_ptr->timer.start(1000 / m_fps);
                }
                initializeMatSize();

            } else
                qDebug() << "Open CV error : Could not open capture : " << m_device;
        }
        emit deviceChanged();
    }
}

void QCamCapture::setFps(qreal fps){
    if ( fps != m_fps ){
        m_fps = fps;
        if ( !m_paused && d_ptr->capture->isOpened() ){
            if ( d_ptr->timer.isActive() )
                d_ptr->timer.stop();
            d_ptr->timer.start(1000 / m_fps);
        }
        emit fpsChanged();
    }
}


void QCamCapture::switchMat(){
    QMat* tempSwitch;
    tempSwitch = d_ptr->inactiveMat;
    d_ptr->inactiveMat = output();
    d_ptr->inactiveMatReady = false;
    setOutput(tempSwitch);
    update();
}

void QCamCapture::setPaused(bool paused){
    if ( m_paused != paused ){
        m_paused = paused;
        if ( m_paused ){
            d_ptr->timer.stop();
        } else
            d_ptr->timer.start(m_fps);
        emit pausedChanged();
    }
}

QCamCapture::~QCamCapture(){
    //d_ptr->capture->release();
    d_ptr->timer.stop();
    delete d_ptr;
}

void QCamCapture::initializeMatSize(){
    int frameW = (int)d_ptr->capture->get(CV_CAP_PROP_FRAME_WIDTH);
    int frameH = (int)d_ptr->capture->get(CV_CAP_PROP_FRAME_HEIGHT);
    if ( frameW == 0 || frameH == 0 ){ // Sacrifice one frame to get width and height
        if ( d_ptr->capture->grab() ){
            cv::Mat firstFrame;
            d_ptr->capture->retrieve(firstFrame);
            frameW = firstFrame.size().width;
            frameH = firstFrame.size().height;
        }
    }

    setImplicitWidth(frameW);
    setImplicitHeight(frameH);
}
