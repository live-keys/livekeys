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

#include "qvideowriter.h"
#include "qvideowriterthread.h"
#include "live/staticcontainer.h"
#include <QJSValueIterator>
#include <QQmlContext>
#include "opencv2/highgui.hpp"

QVideoWriter::QVideoWriter(QObject *parent)
    : QObject(parent)
    , m_thread(nullptr)
    , m_stream(new lv::QmlWritableStream(this))
{

}

QVideoWriter::~QVideoWriter(){
}

int QVideoWriter::framesWritten() const{
    return m_thread ? m_thread->framesWritten() : 0;
}

void QVideoWriter::onInStream(QObject *that, const QJSValue &val){
    QVideoWriter* ethat = static_cast<QVideoWriter*>(that);
    QMat* m = qobject_cast<QMat*>(val.toQObject());
    if ( m ){
        ethat->write(m);
    }
}

void QVideoWriter::setOptions(QJSValue options){
    QString filename = "";
    int fourcc = 0;
    double fps = 0;
    cv::Size frameSize = cv::Size(0, 0);
    int isColor = true;

    QJSValueIterator initIt(options);
    while ( initIt.hasNext() ){
        initIt.next();
        if ( initIt.name() == "filename" ){
            filename = initIt.value().toString();
        } else if ( initIt.name() == "fourcc" ){
            if ( initIt.value().isNumber() ){
                fourcc = initIt.value().toInt();
            } else {
                QString fourccstr = initIt.value().toString();
                if ( fourccstr.length() != 4 ){
                    qWarning("Invalid fourcc codec length. VideoWriter will not work correctly.");
                } else {
                    fourcc = cv::VideoWriter::fourcc(
                        fourccstr[0].toLatin1(),
                        fourccstr[1].toLatin1(),
                        fourccstr[2].toLatin1(),
                        fourccstr[3].toLatin1()
                    );
                }
            }
        } else if ( initIt.name() == "fps" ){
            fps = initIt.value().toNumber();
        } else if ( initIt.name() == "frameWidth" ){
            frameSize.width = initIt.value().toInt();
        } else if ( initIt.name() == "frameHeight" ){
            frameSize.height = initIt.value().toInt();
        } else if ( initIt.name() == "isColor" ){
            isColor = initIt.value().toBool();
        }
    }

    m_thread = createThread(filename, fourcc, fps, frameSize, isColor);
    m_thread->start();

    m_options = options;
    emit optionsChanged();
}

void QVideoWriter::save(){
    if ( m_thread )
        m_thread->save();
}

void QVideoWriter::write(QMat *image){
    if ( m_thread ){
        if ( !m_thread->isOpen() ){
            m_thread->open();
        }
        m_thread->write(image);
    }
}

QVideoWriterThread *QVideoWriter::createThread(const QString &filename, int fourcc, double fps, const cv::Size frameSize, bool isColor){
    return new QVideoWriterThread(
        filename,
        fourcc,
        fps,
        frameSize,
        isColor,
        0
    );
}
