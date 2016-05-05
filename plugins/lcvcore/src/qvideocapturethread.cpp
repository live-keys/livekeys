/****************************************************************************
**
** Copyright (C) 2014-2016 Dinu SV.
** (contact: mail@dinusv.com)
** This file is part of Live CV Application.
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

#include "qvideocapturethread.h"
#include "qmat.h"

#include "opencv2/video/video.hpp"
#include "opencv2/highgui/highgui.hpp"

#include <QMutex>
#include <QTime>
#include <QTimer>
#include <QReadWriteLock>
#include <QWaitCondition>


#define QVIDEO_CAPTURE_THREAD_DEBUG_FLAG
#ifdef QVIDEO_CAPTURE_THREAD_DEBUG_FLAG
#define QVIDEO_CAPTURE_THREAD_DEBUG(_param) qDebug() << (_param)
#else
#define QVIDEO_CAPTURE_THREAD_DEBUG(_param)
#endif


using namespace cv;

class QVideoCaptureThreadPrivate{

public:
    VideoCapture*  capture;
    QMat*          inactiveMat;

    bool           abord;
    bool           inactiveMatReady;

    int            width;
    int            height;

    int            seekRequest;

    QMutex         mutex;
    QWaitCondition condition;

};


/*!
  \class QVideoCaptureThread
  \internal
  \brief Internal video capture thread used by QVideoCapture.
 */

QVideoCaptureThread::QVideoCaptureThread(const QString &file, QObject *parent) :
    QThread(parent),
    m_file(file),
    m_paused(false),
    m_framePos(0),
    m_totalFrames(0),
    m_isSeeking(false),
    m_forceSeek(false),
    m_activeMat(new QMat),
    d_ptr(new QVideoCaptureThreadPrivate)
{
    Q_D(QVideoCaptureThread);
    d->inactiveMat      = new QMat;
    d->inactiveMatReady = false;
    d->seekRequest      = -1;
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
    QVIDEO_CAPTURE_THREAD_DEBUG( QString("Video capture \"") + m_file + "\" thread released." );
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

    if ( d->capture->isOpened() )
        initializeMatSize();
    return d->capture->isOpened();
}

void QVideoCaptureThread::processNextFrame(){
    Q_D(QVideoCaptureThread);
    d->inactiveMatReady = false;
}

void QVideoCaptureThread::setLoop(bool loop){
    m_loop = loop;
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

void QVideoCaptureThread::seekTo(int frame){
    Q_D(QVideoCaptureThread);
    if ( frame != m_framePos && m_totalFrames != 0 ){
        d->seekRequest = frame;

        //tick();
    }
}


void QVideoCaptureThread::run(){
    Q_D(QVideoCaptureThread);

    d->abord = false;

    forever{

        if ( d->seekRequest != -1 ){
            if ( m_totalFrames == 0 )
                qWarning("Error (VideoCapture): Seek is not available for this video.");
            else {
                if ( d->seekRequest != m_framePos ){
                    QVIDEO_CAPTURE_THREAD_DEBUG("Seek request");
                    beginSeek();
                    d->capture->set(CV_CAP_PROP_POS_FRAMES, d->seekRequest);
                    m_framePos = (int)d->capture->get(CV_CAP_PROP_POS_FRAMES);
                    if ( m_framePos != d->seekRequest && m_forceSeek ){
                        d->capture->set(CV_CAP_PROP_POS_FRAMES, 0);
                        int i = 0;
                        while ( i < d->seekRequest )
                            d->capture->grab();
                    }
                    endSeek();
                }
            }
            d->seekRequest = -1;
        }

        if ( d->capture->grab() ){

            d->capture->retrieve(*d_ptr->inactiveMat->cvMat());
            d->inactiveMatReady = true;
            QMat* tempSwitch;
            tempSwitch      = d->inactiveMat;
            d->inactiveMat  = m_activeMat;
            m_activeMat     = tempSwitch;
            ++m_framePos;
            emit inactiveMatChanged();
        } else {
            if ( m_loop ){
                qWarning("Open CV Error: No image captured, restarting stream..");
                d->seekRequest = 0;
            } else {
                qWarning("Open CV Error: No image captured.");
            }
        }

        d->mutex.lock();
        if ( d->inactiveMatReady && !d->abord )
            d->condition.wait(&d->mutex);
        if ( d->abord ){
            d->mutex.unlock();
            return;
        }
        d->mutex.unlock();
    }
}

void QVideoCaptureThread::initializeMatSize(){
    Q_D(QVideoCaptureThread);
    m_totalFrames = (int)d->capture->get(CV_CAP_PROP_FRAME_COUNT);
    d->width      = (int)d->capture->get(CV_CAP_PROP_FRAME_WIDTH);
    d->height     = (int)d->capture->get(CV_CAP_PROP_FRAME_HEIGHT);
    if ( d->width == 0 || d->height == 0 ){ // Sacrifice one frame to get width and height
        if ( d->capture->grab() ){
            cv::Mat firstFrame;
            d->capture->retrieve(firstFrame);
            d->width  = firstFrame.size().width;
            d->height = firstFrame.size().height;
            ++m_framePos;
        }
    }
}

void QVideoCaptureThread::beginSeek(){
    m_isSeeking = true;
    emit isSeekingChanged();
}

void QVideoCaptureThread::endSeek(){
    m_isSeeking = false;
    emit isSeekingChanged();
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
