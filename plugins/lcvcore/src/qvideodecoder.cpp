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

#include "qvideodecoder.h"
#include "qvideodecodethread.h"
#include "qmat.h"

#include "live/viewcontext.h"
#include "live/viewengine.h"
#include "live/visuallogqt.h"
#include "live/exception.h"

#include <QFile>
#include <QTimer>
#include <QDebug>

const int QVideoDecoder::FramesToGC = 10;

// class QVideoDecoder::Properties
// -------------------------------

QVideoDecoder::Properties::Properties()
    : totalFrames(0)
    , currentFrame(0)
    , fps(0)
    , paused(false)
    , loop(false)
    , isSeeking(false)
    , forceSeek(false)
{
}

// class QVideoDecoder
// -------------------

QVideoDecoder::QVideoDecoder(QObject *parent)
    : QObject(parent)
    , m_worker(nullptr)
    , m_stream(nullptr)
    , m_properties(new QVideoDecoder::Properties)
    , m_decodedFramesToGC(0)
{
}

QVideoDecoder::~QVideoDecoder(){
    delete m_worker;
    delete m_stream;
    delete m_properties;
}

void QVideoDecoder::setPaused(bool paused){
    if ( m_properties->paused == paused )
        return;

    if ( m_worker ){
        if ( paused ){
            m_worker->timer()->stop();
        } else {
            m_worker->timer()->start(static_cast<int>(1000 / runningFps()));
        }
    }

    m_properties->paused = paused;
    emit pausedChanged();
}


void QVideoDecoder::setFps(qreal fps){
    if ( qFuzzyCompare(m_properties->fps, fps ) )
        return;

    m_properties->fps = fps;

    if ( m_worker ){
        if ( !m_properties->paused && m_worker->isCaptureOpened() ){
            if ( m_worker->timer()->isActive() )
                m_worker->timer()->stop();
            m_worker->timer()->start(static_cast<int>(1000 / runningFps()));
        }
    }

    emit fpsChanged();
}

void QVideoDecoder::setLoop(bool loop){
    if ( m_properties->loop == loop )
        return;

    m_properties->loop = loop;
    emit loopChanged();
}

qreal QVideoDecoder::runningFps() const{
    if ( !m_worker )
        return 0;

    return qFuzzyCompare(m_properties->fps, 0) ? m_worker->captureFps() : m_properties->fps;
}

void QVideoDecoder::seekTo(int frame){
    if ( m_properties->currentFrame == frame )
        return;

    if ( m_worker && frame != m_properties->currentFrame ){
        m_worker->seekTo(frame);
    }
}

lv::QmlStream* QVideoDecoder::run(const QString &file){
    if ( m_worker ){
        if ( m_worker->file() == file ){
            return m_stream;
        }

        if ( !QFile::exists(file) ){
            lv::ViewEngine* ve = lv::ViewEngine::grab(this);
            if ( ve ){
                lv::QmlError(
                    ve,
                    CREATE_EXCEPTION(lv::Exception, "VideoDecoder: File does not exist: " + file.toStdString() + ".", lv::Exception::toCode("~Mismatch")),
                    this
                ).jsThrow();
            }
            return m_stream;
        }

        delete m_worker;
        delete m_stream;
    }

    m_worker = new QVideoDecodeThread(file, m_properties, this);
    m_stream = new lv::QmlStream(this);
    connect(m_worker, &QVideoDecodeThread::matReady, this, &QVideoDecoder::__matReady);

    if ( m_worker->isCaptureOpened() ){

        double fps = qFuzzyCompare(m_properties->fps, 0) ? m_worker->captureFps() : m_properties->fps;

        if ( !m_properties->paused ){
            if ( m_worker->timer()->isActive() ){
                if ( !qFuzzyCompare(m_worker->timer()->interval(), (1000 / fps) ) ){
                    m_worker->timer()->stop();
                    m_worker->timer()->start(static_cast<int>(1000 / fps));
                }
            } else {
                m_worker->timer()->start(static_cast<int>(1000 / fps));
            }
        }
        emit streamChanged();
        emit totalFramesChanged();

    } else {
        lv::ViewEngine* ve = lv::ViewEngine::grab(this);
        if ( ve ){
            lv::QmlError(
                ve,
                CREATE_EXCEPTION(lv::Exception, "VideoDecoder: Could not decode file: " + file.toStdString() + ".", lv::Exception::toCode("~Mismatch")),
                this
            ).jsThrow();
        }
    }

    return m_stream;
}

void QVideoDecoder::__matReady(){
    if ( m_worker ){
        QMat* m = m_worker->takeMat();
        lv::Shared::ownJs(m);
        m_stream->push(m);
        emit currentFrameChanged();
        ++m_decodedFramesToGC;
        if ( m_decodedFramesToGC > QVideoDecoder::FramesToGC ){
            lv::ViewEngine* engine = lv::ViewEngine::grab(this);
            engine->engine()->collectGarbage();
            m_decodedFramesToGC = 0;
        }
        m_worker->processNextFrame();
    }
}

QString QVideoDecoder::file() const{
    if ( m_worker )
        return m_worker->file();
    return QString();
}
