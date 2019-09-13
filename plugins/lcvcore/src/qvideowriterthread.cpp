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

#include "qvideowriterthread.h"
#include "qvideowriter.h"

#include <QWaitCondition>

/*!
  \class QVideoWriterThread
  \internal
  \brief Internal video writer thread used by QVideoWriter.
 */

QVideoWriterThread::QVideoWriterThread(
        const QString filename,
        int fourcc,
        double fps,
        const cv::Size &frameSize,
        bool isColor,
        QObject *parent)
    : QThread(parent)
    , m_filename(filename)
    , m_framesWritten(0)
    , m_fourcc(fourcc)
    , m_fps(fps)
    , m_frameSize(frameSize)
    , m_isColor(isColor)
{
    m_writer = new cv::VideoWriter();
    m_writer->open(filename.toStdString(), fourcc, fps, frameSize, isColor);
    if ( !m_writer->isOpened() ){
        qWarning("Failed to open VideoWriter on file: %s", qPrintable(filename));
    }
}

QVideoWriterThread::~QVideoWriterThread(){
    stop();
    m_mutex.lock();
    m_hasData.wakeAll();
    m_mutex.unlock();
    if ( !wait(5000) ){
        qCritical("VideoWriter Thread failed to close, forcing quit. This may lead to inconsistent application state.");
        terminate();
        wait();
    }
    m_writer->release();
    delete m_writer;
}

void QVideoWriterThread::run(){
    m_stop = false;
    while( !m_stop ){
        m_mutex.lock();
        if( !m_hasDataToWrite )
            m_hasData.wait(&m_mutex);
        m_mutex.unlock();

        m_mutex.lock();

        if ( m_hasDataToWrite ){

            m_writer->write(m_data);
            ++m_framesWritten;

            QVideoWriter* wrparent = qobject_cast<QVideoWriter*>(parent());
            if ( wrparent )
                emit wrparent->framesWrittenChanged();

            m_hasDataToWrite = false;
        }

        m_mutex.unlock();
    }
}

void QVideoWriterThread::stop(){
    m_stop = true;
}

void QVideoWriterThread::save(){
    m_mutex.lock();
    m_writer->release();
    m_mutex.unlock();
}

bool QVideoWriterThread::isOpen(){
    return m_writer->isOpened();
}

void QVideoWriterThread::open(){
    m_writer->open(m_filename.toStdString(), m_fourcc, m_fps, m_frameSize, m_isColor);
    if ( !m_writer->isOpened() ){
        qWarning("Failed to open VideoWriter on file: %s", qPrintable(m_filename));
    }
}

void QVideoWriterThread::write(QMat *mat){
    m_mutex.lock();
    mat->cvMat()->copyTo(m_data);
    m_hasDataToWrite = true;
    m_hasData.wakeAll();
    m_mutex.unlock();
}
