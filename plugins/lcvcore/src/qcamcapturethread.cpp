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

#include "qcamcapturethread.h"
#include "qmat.h"

#include "opencv2/video/video.hpp"
#include "opencv2/highgui/highgui.hpp"

#include <QMutex>
#include <QTimer>
#include <QReadWriteLock>
#include <QWaitCondition>

#define QCAM_CAPTURE_THREAD_DEBUG_FLAG
#ifdef QCAM_CAPTURE_THREAD_DEBUG_FLAG
#define QCAM_CAPTURE_THREAD_DEBUG(_param) qDebug() << (_param)
#else
#define QCAM_CAPTURE_THREAD_DEBUG(_param)
#endif

using namespace cv;

class QCamCaptureThreadPrivate{

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

/*!
  \class QCamCaptureThread
  \internal
  \brief Camera capture thread used by QCamCapture.
 */

QCamCaptureThread::QCamCaptureThread(const QString& device, QObject *parent) :
    QThread(parent),
    m_deviceId(device),
    m_paused(false),
    m_activeMat(new QMat),
    d_ptr(new QCamCaptureThreadPrivate)
{
    Q_D(QCamCaptureThread);
    d->inactiveMat      = new QMat;
    d->inactiveMatReady = false;
    d->capture          = new VideoCapture();
    openCapture();

    m_timer             = new QTimer;
    connect(m_timer, SIGNAL(timeout()), this, SLOT(tick()));
}

QCamCaptureThread::~QCamCaptureThread(){
    Q_D(QCamCaptureThread);
    d->mutex.lock();
    d->abord = true;
    d->condition.wakeOne();
    d->mutex.unlock();
    wait(); // wait till thread finishes]
    QCAM_CAPTURE_THREAD_DEBUG(QString("Cam capture \"") + m_deviceId + "\" thread released.");
    d->capture->release();
    delete m_timer;
    delete d->inactiveMat;
    delete d->capture;
    delete d;
}

bool QCamCaptureThread::isCaptureOpened(){
    Q_D(QCamCaptureThread);
    if ( !d->capture->isOpened() )
        d->capture->open(m_deviceId.toStdString());
    return d->capture->isOpened();
}

void QCamCaptureThread::processNextFrame(){
    Q_D(QCamCaptureThread);
    d->inactiveMatReady = false;
}

void QCamCaptureThread::tick(){
    Q_D(QCamCaptureThread);
    QMutexLocker lock(&d->mutex);
    if ( !isRunning() ){
        start(LowPriority);
    } else {
        if ( !d->inactiveMatReady ){
            d->condition.wakeOne();
        }
    }
}

void QCamCaptureThread::run(){
    Q_D(QCamCaptureThread);
    forever{
        if ( d->capture->grab() ){
            d->capture->retrieve(*d_ptr->inactiveMat->cvMat());
            d->inactiveMatReady = true;
            QMat* tempSwitch;
            tempSwitch      = d->inactiveMat;
            d->inactiveMat  = m_activeMat;
            m_activeMat     = tempSwitch;
            emit inactiveMatChanged();
        } else
            qWarning("Open CV Error: No image captured");

        d->mutex.lock();
        if ( d->inactiveMatReady && !d->abord){
            d->condition.wait(&d->mutex);
        }
        d->inactiveMatReady = true;
        if ( d->abord ){
            d->mutex.unlock();
            return;
        }
        d->mutex.unlock();
    }
}

void QCamCaptureThread::initializeMatSize(){
    Q_D(QCamCaptureThread);
    d->width = (int)d->capture->get(CV_CAP_PROP_FRAME_WIDTH);
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

void QCamCaptureThread::openCapture(){
    Q_D(QCamCaptureThread);
    bool convertOk;
    int deviceInt = m_deviceId.toInt(&convertOk);
    if ( convertOk )
        d->capture->open(deviceInt);
    else
        d->capture->open(m_deviceId.toStdString());
    if ( d->capture->isOpened() )
        initializeMatSize();
}

int QCamCaptureThread::captureWidth() const{
    Q_D(const QCamCaptureThread);
    return d->width;
}

int QCamCaptureThread::captureHeight() const{
    Q_D(const QCamCaptureThread);
    return d->height;
}
