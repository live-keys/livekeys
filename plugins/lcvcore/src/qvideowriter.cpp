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
#include "qstaticcontainer.h"
#include <QJSValueIterator>
#include <QQmlContext>
#include "opencv2/highgui.hpp"

//TODO: Document

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

QString QVideoWriter::getKey(const QString &filename,
        int fourcc,
        double fps,
        const cv::Size frameSize) const
{
    return
        filename +
        QString::number(fourcc) +
        QString::number(fps) +
        QString::number(frameSize.width) +
        QString::number(frameSize.height);
}

void QVideoWriter::componentComplete(){
    QQuickItem::componentComplete();
}

void QVideoWriter::staticLoad(const QJSValue &params){
    QString filename = "";
    int m_fourcc = 0;
    double m_fps = 0;
    cv::Size m_frameSize = cv::Size(0, 0);
    int m_isColor = true;

    QJSValueIterator initIt(params);
    while ( initIt.hasNext() ){
        initIt.next();
        if ( initIt.name() == "filename" ){
            filename = initIt.value().toString();
        } else if ( initIt.name() == "fourcc" ){
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

    QStaticContainer* container = qmlContext(this)->contextProperty("staticContainer").value<QStaticContainer*>();
    m_thread = container->get<QVideoWriterThread>(getKey(
        filename, m_fourcc, m_fps, m_frameSize
    ));

    if ( !m_thread ){
        m_thread = createThread(filename, m_fourcc, m_fps, m_frameSize, m_isColor);
        container->set<QVideoWriterThread>(getKey(
            filename, m_fourcc, m_fps, m_frameSize
        ), m_thread);
    }
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
        this
    );
}
