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

#ifndef QVIDEOWRITERTHREAD_H
#define QVIDEOWRITERTHREAD_H

#include <QThread>
#include <QMutex>
#include <QWaitCondition>
#include "qmat.h"
#include "opencv2/highgui.hpp"

class QVideoWriterThread : public QThread{

    Q_OBJECT

public:
    QVideoWriterThread(
        const QString filename,
        int fourcc,
        double fps,
        const cv::Size& frameSize,
        bool isColor,
        QObject* parent = 0
    );
    ~QVideoWriterThread();

    void run() Q_DECL_OVERRIDE;
    void stop();
    void save();
    bool isOpen();
    void open();

    void write(QMat* mat);
    int framesWritten() const;

private:
    QString          m_filename;
    QMutex           m_mutex;
    QWaitCondition   m_hasData;
    cv::VideoWriter* m_writer;
    int              m_framesWritten;
    cv::Mat          m_data;
    bool             m_hasDataToWrite;
    bool             m_stop;

    int              m_fourcc;
    double           m_fps;
    cv::Size         m_frameSize;
    bool             m_isColor;
};

inline int QVideoWriterThread::framesWritten() const{
    return m_framesWritten;
}

#endif // QVIDEOWRITERTHREAD_H
