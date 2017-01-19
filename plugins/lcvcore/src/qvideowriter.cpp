/****************************************************************************
**
** Copyright (C) 2014-2017 Dinu SV.
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

#include "qvideowriter.h"
#include "qvideowriterthread.h"
#include "qstatecontainer.h"
#include <QJSValueIterator>
#include "opencv2/highgui/highgui.hpp"

QVideoWriter::QVideoWriter(QQuickItem *parent)
    : QQuickItem(parent)
    , m_input(QMat::nullMat())
    , m_thread(0)
{
    QQuickItem::setFlag(QQuickItem::ItemHasContents, false);
}

QVideoWriter::~QVideoWriter(){
}

int QVideoWriter::framesWritten() const{
    return m_thread ? m_thread->framesWritten() : 0;
}

QString QVideoWriter::getKey() const{
    return
        m_filename +
        QString::number(m_fourcc) +
        QString::number(m_fps) +
        QString::number(m_frameSize.width) +
        QString::number(m_frameSize.height);
}

void QVideoWriter::componentComplete(){
    QQuickItem::componentComplete();

    if ( !m_init.isNull() && m_fourcc != 0 && !m_filename.isEmpty() ){
        QStateContainer<QVideoWriterThread>& stateCont =
            QStateContainer<QVideoWriterThread>::instance(this);

        m_thread = stateCont.state(getKey());
        if ( m_thread == 0 ){
            m_thread = createThread();
            m_thread->start();
            stateCont.registerState(getKey(), m_thread);
        }
    }
}

void QVideoWriter::setInit(const QJSValue &init){
    m_fourcc = 0;
    m_fps = 0;
    m_frameSize = cv::Size(0, 0);
    m_isColor = true;

    QJSValueIterator initIt(init);
    while ( initIt.hasNext() ){
        initIt.next();
        if ( initIt.name() == "fourcc" ){
            if ( initIt.value().isNumber() ){
                m_fourcc = initIt.value().toInt();
            } else {
                QString fourccstr = initIt.value().toString();
                if ( fourccstr.length() != 4 ){
                    qWarning("Invalid fourcc codec length. VideoWriter will not work correctly.");
                } else {
                    m_fourcc = CV_FOURCC(
                        fourccstr[0].toLatin1(),
                        fourccstr[1].toLatin1(),
                        fourccstr[2].toLatin1(),
                        fourccstr[3].toLatin1()
                    );
                }
            }
        } else if ( initIt.name() == "fps" ){
            m_fps = initIt.value().toNumber();
        } else if ( initIt.name() == "frameWidth" ){
            m_frameSize.width = initIt.value().toInt();
        } else if ( initIt.name() == "frameHeight" ){
            m_frameSize.height = initIt.value().toInt();
        } else if ( initIt.name() == "isColor" ){
            m_isColor = initIt.value().toBool();
        }
    }

    m_init = init;
    emit initChanged();

    if ( m_fourcc != 0 && isComponentComplete() ){
        delete m_thread;
        QStateContainer<QVideoWriterThread>& stateCont =
            QStateContainer<QVideoWriterThread>::instance(this);

        m_thread = createThread();
        stateCont.registerState(m_filename, m_thread);
    }
}

void QVideoWriter::setFilename(const QString &filename){
    if (m_filename == filename)
        return;

    m_filename = filename;
    if ( isComponentComplete() ){
        QStateContainer<QVideoWriterThread>& stateCont =
            QStateContainer<QVideoWriterThread>::instance(this);

        m_thread = stateCont.state(getKey());
        if ( m_thread == 0 ){
            m_thread = createThread();
            stateCont.registerState(getKey(), m_thread);
        }
    }

    emit filenameChanged();
}

void QVideoWriter::save(){
    if ( m_thread )
        m_thread->save();
}

void QVideoWriter::write(QMat *image){
    if ( m_thread ){
        if ( !m_thread->isOpen() ){
            m_thread->open(m_filename, m_fourcc, m_fps, m_frameSize, m_isColor);
        }
        m_thread->write(image);
    }
}

QVideoWriterThread *QVideoWriter::createThread(){
    return new QVideoWriterThread(
        m_filename,
        m_fourcc,
        m_fps,
        m_frameSize,
        m_isColor,
        this
    );
}
