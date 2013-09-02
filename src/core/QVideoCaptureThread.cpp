#include "QVideoCaptureThread.hpp"
#include "QMat.hpp"

#include "opencv2/video/video.hpp"
#include "opencv2/highgui/highgui.hpp"

#include <QTimer>
#include <QReadWriteLock>
#include <QWaitCondition>

using namespace cv;

class QVideoCaptureThreadPrivate{

public:
    VideoCapture*  capture;
    QMat*          inactiveMat;

    bool           abord;
    bool           inactiveMatReady;

    int            width;
    int            height;

    QMutex         mutex;
    QWaitCondition condition;

};

QVideoCaptureThread::QVideoCaptureThread(const QString &file, QObject *parent) :
    QThread(parent),
    m_file(file),
    m_activeMat(new QMat),
    d_ptr(new QVideoCaptureThreadPrivate)
{
    Q_D(QVideoCaptureThread);
    d->inactiveMat      = new QMat;
    d->inactiveMatReady = false;
    d->capture          = new VideoCapture(file.toStdString());
    if ( d->capture->isOpened() )
        initializeMatSize();

    m_timer             = new QTimer;
    connect(m_timer, SIGNAL(timeout()), this, SLOT(tick()));
}

QVideoCaptureThread::~QVideoCaptureThread(){
    Q_D(QVideoCaptureThread);
    d->mutex.lock();
    d->abord = true;
    d->condition.wakeOne();
    d->mutex.unlock();
    wait(); // wait till thread finishes
    d->capture->release();
    delete m_timer;
    delete d->inactiveMat;
    delete d->capture;
    delete d;
}

bool QVideoCaptureThread::isCaptureOpened(){
    Q_D(QVideoCaptureThread);
    if ( !d->capture->isOpened() )
        d->capture->open(file().toStdString());
    return d->capture->isOpened();
}

void QVideoCaptureThread::processNextFrame(){
    Q_D(QVideoCaptureThread);
    d->inactiveMatReady = false;
}

void QVideoCaptureThread::tick(){
    Q_D(QVideoCaptureThread);
    QMutexLocker lock(&d->mutex);
    if ( !isRunning() ){
        start(NormalPriority);
    } else {
        if ( !d->inactiveMatReady ){
            d->condition.wakeOne();
        }
    }
}

#include <QTime>

void QVideoCaptureThread::run(){
    Q_D(QVideoCaptureThread);
    forever{
        if ( d->capture->grab() ){
            d->capture->retrieve(*d_ptr->inactiveMat->data());
            d->inactiveMatReady = true;
            QMat* tempSwitch;
            tempSwitch      = d->inactiveMat;
            d->inactiveMat  = m_activeMat;
            m_activeMat     = tempSwitch;
            emit inactiveMatChanged();
        } else
            qDebug() << "Open CV : No image captured";

        d->mutex.lock();
        if ( d->inactiveMatReady )
            d->condition.wait(&d_ptr->mutex);
        d->inactiveMatReady = true;
        if ( d->abord ){
            d->mutex.unlock();
            return;
        }
        d->mutex.unlock();
    }
}

void QVideoCaptureThread::initializeMatSize(){
    Q_D(QVideoCaptureThread);
    d->width  = (int)d->capture->get(CV_CAP_PROP_FRAME_WIDTH);
    d->height = (int)d->capture->get(CV_CAP_PROP_FRAME_HEIGHT);
    if ( d->width == 0 || d->height == 0 ){ // Sacrifice one frame to get width and height
        if ( d->capture->grab() ){
            cv::Mat firstFrame;
            d->capture->retrieve(firstFrame);
            d->width  = firstFrame.size().width;
            d->height = firstFrame.size().height;
        }
    }
}

int QVideoCaptureThread::captureWidth() const{
    Q_D(const QVideoCaptureThread);
    return d->width;
}

int QVideoCaptureThread::captureHeight() const{
    Q_D(const QVideoCaptureThread);
    return d->height;
}

double QVideoCaptureThread::captureFps() const{
    Q_D(const QVideoCaptureThread);
    if ( d->capture->isOpened())
        return d->capture->get(CV_CAP_PROP_FPS);
    return 0;
}
