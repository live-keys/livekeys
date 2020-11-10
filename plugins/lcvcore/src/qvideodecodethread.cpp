/****************************************************************************
**
** Copyright (C) 2014-2019 Dinu SV.
** (contact: mail@dinusv.com)
** This file is part of Livekeys Application.
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

#include "qvideodecodethread.h"

#include "qmat.h"

#include "live/visuallog.h"

#include "opencv2/video.hpp"
#include "opencv2/highgui.hpp"

#include <QMutex>
#include <QTime>
#include <QTimer>
#include <QReadWriteLock>
#include <QWaitCondition>

#include "live/memory.h"

using namespace cv;
/// \private
class QVideoDecodeThreadPrivate{

public:
    QString        file;

    VideoCapture*  capture;
    QMat*          inactiveMat;
    QMat*          activeMat;
    QTimer*        timer;

    bool           abord;
    bool           inactiveMatReady;

    int            captureWidth;
    int            captureHeight;

    int            seekRequest;

    QMutex         mutex;
    QWaitCondition condition;
};


QVideoDecodeThread::QVideoDecodeThread(const QString &file, QVideoDecoder::Properties *properties, QObject *parent)
    : QThread(parent)
    , m_properties(properties)
    , d_ptr(new QVideoDecodeThreadPrivate)
{

    Q_D(QVideoDecodeThread);
    d->file             = file;

    d->activeMat = nullptr;

    d->inactiveMatReady = false;
    d->seekRequest      = -1;
    d->capture          = new VideoCapture(file.toStdString());
    d->captureWidth     = 0;
    d->captureHeight    = 0;
    if ( d->capture->isOpened() )
        initializeMatSize();

    d->inactiveMat = new QMat(d->captureWidth, d->captureHeight, QMat::CV8U, 3);
    lv::Memory::reserve<QMat, cv::Mat>(d->inactiveMat);

    d->timer = new QTimer;
    connect(d->timer, SIGNAL(timeout()), this, SLOT(tick()));
}


QVideoDecodeThread::~QVideoDecodeThread(){
    Q_D(QVideoDecodeThread);
    d->mutex.lock();
    d->abord = true;
    d->condition.wakeOne();
    d->mutex.unlock();
    wait(); // wait till thread finishes
    vlog_debug("cv-videocapture",  QString("Video capture \"") + m_file + "\" thread released." );
    d->capture->release();
    delete d->timer;
    delete d->inactiveMat;
    delete d->capture;
    delete d;
}

bool QVideoDecodeThread::isCaptureOpened(){
    Q_D(QVideoDecodeThread);
    if ( !d->capture->isOpened() )
        d->capture->open(file().toStdString());

    if ( d->capture->isOpened() )
        initializeMatSize();
    return d->capture->isOpened();
}

void QVideoDecodeThread::processNextFrame(){
    Q_D(QVideoDecodeThread);
    d->inactiveMatReady = false;
    d->inactiveMat = new QMat(d->captureWidth, d->captureHeight, QMat::CV8U, 3);
}

void QVideoDecodeThread::tick(){
    Q_D(QVideoDecodeThread);
    QMutexLocker lock(&d->mutex);
    if ( !isRunning() ){
        start(NormalPriority);
    } else {
        if ( !d->inactiveMatReady ){
            d->condition.wakeOne();
        }
    }
}

void QVideoDecodeThread::seekTo(int frame){
    Q_D(QVideoDecodeThread);
    if ( frame != m_properties->currentFrame && m_properties->totalFrames != 0 ){
        d->seekRequest = frame;
        //tick();
    }
}


void QVideoDecodeThread::run(){
    Q_D(QVideoDecodeThread);
    QVideoDecoder::Properties* p = m_properties;

    d->abord = false;

    forever{

        if ( d->seekRequest != -1 ){
            if ( p->totalFrames == 0 )
                qWarning("Error (VideoCapture): Seek is not available for this video.");
            else {
                if ( d->seekRequest != p->currentFrame ){
                    vlog_debug("cv-videocapture", "Seek request");
                    beginSeek();
                    d->capture->set(cv::CAP_PROP_POS_FRAMES, d->seekRequest);
                    p->currentFrame = (int)d->capture->get(cv::CAP_PROP_POS_FRAMES);
                    if ( p->currentFrame != d->seekRequest && p->forceSeek ){
                        d->capture->set(cv::CAP_PROP_POS_FRAMES, 0);
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

            d->capture->retrieve(*d_ptr->inactiveMat->internalPtr());
            d->inactiveMatReady = true;
            QMat* tempSwitch;
            tempSwitch      = d->inactiveMat;
            d->inactiveMat  = d->activeMat;
            d->activeMat     = tempSwitch;
            ++p->currentFrame;
            emit matReady();
        } else {
            if ( p->totalFrames != 0 && p->currentFrame != p->totalFrames ){
                if ( p->loop ){
                    qWarning("Open CV Error: No image captured, restarting stream..");
                } else {
                    qWarning("Open CV Error: No image captured.");
                }
            }

            if ( p->loop ){
                d->seekRequest = 0;
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

void QVideoDecodeThread::initializeMatSize(){
    Q_D(QVideoDecodeThread);
    m_properties->totalFrames = (int)d->capture->get(cv::CAP_PROP_FRAME_COUNT);
    d->captureWidth  = (int)d->capture->get(cv::CAP_PROP_FRAME_WIDTH);
    d->captureHeight = (int)d->capture->get(cv::CAP_PROP_FRAME_HEIGHT);
    if ( d->captureWidth == 0 || d->captureHeight == 0 ){ // Sacrifice one frame to get width and height
        if ( d->capture->grab() ){
            cv::Mat firstFrame;
            d->capture->retrieve(firstFrame);
            d->captureWidth  = firstFrame.size().width;
            d->captureHeight = firstFrame.size().height;
            ++m_properties->currentFrame;
        }
    }
}

void QVideoDecodeThread::beginSeek(){
    m_properties->isSeeking = true;
    emit isSeekingChanged();
}

void QVideoDecodeThread::endSeek(){
    m_properties->isSeeking = false;
    emit isSeekingChanged();
}

int QVideoDecodeThread::captureWidth() const{
    Q_D(const QVideoDecodeThread);
    return d->captureWidth;
}

int QVideoDecodeThread::captureHeight() const{
    Q_D(const QVideoDecodeThread);
    return d->captureHeight;
}

double QVideoDecodeThread::captureFps() const{
    Q_D(const QVideoDecodeThread);
    if ( d->capture->isOpened())
        return d->capture->get(cv::CAP_PROP_FPS);
    return 0;
}

QTimer *QVideoDecodeThread::timer() const{
    Q_D(const QVideoDecodeThread);
    return d->timer;
}

const QString& QVideoDecodeThread::file() const{
    Q_D(const QVideoDecodeThread);
    return d->file;
}

QMat *QVideoDecodeThread::output(){
    Q_D(const QVideoDecodeThread);
    return d->activeMat;
}

QMat *QVideoDecodeThread::takeMat(){
    Q_D(QVideoDecodeThread);
    QMat* r = d->activeMat;
    d->activeMat = nullptr;
    return r;
}
