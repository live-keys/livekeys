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

#ifndef QCAMCAPTURETHREAD_H
#define QCAMCAPTURETHREAD_H

#include <QThread>
#include "qlcvglobal.h"

class QMat;
class QTimer;
class QCamCaptureThreadPrivate;
class QCamCaptureThread: public QThread{

    Q_OBJECT

public:
    QCamCaptureThread(const QString& device, QObject* parent = 0);
    ~QCamCaptureThread();

    QMat*   output();
    QTimer* timer();
    int     captureWidth() const;
    int     captureHeight() const;
    void    setCaptureResolution(int width, int height);
    const QString& device() const;

    bool    isCaptureOpened();

    bool    paused() const;
    void    setPaused(bool paused);

    void    processNextFrame();
signals:
    void inactiveMatChanged();

public slots:
    void tick();

protected:
    void run();

private:
    void initializeMatSize();
    void openCapture();

    QCamCaptureThread(const QCamCaptureThread& other);
    QCamCaptureThread& operator= (const QCamCaptureThread& other);

    QString m_deviceId;
    bool    m_paused;
    QMat*   m_activeMat;
    QTimer* m_timer;

    QCamCaptureThreadPrivate* const d_ptr;

    Q_DECLARE_PRIVATE(QCamCaptureThread)

};

inline QTimer *QCamCaptureThread::timer(){
    return m_timer;
}

inline const QString &QCamCaptureThread::device() const{
    return m_deviceId;
}

inline QMat *QCamCaptureThread::output(){
    return m_activeMat;
}

inline void QCamCaptureThread::setPaused(bool paused){
    m_paused = paused;
}

inline bool QCamCaptureThread::paused() const{
    return m_paused;
}

#endif // QCAMCAPTURETHREAD_H
