/****************************************************************************
**
** Copyright (C) 2014 Dinu SV.
** (contact: mail@dinusv.com)
** This file is part of Live CV application.
**
** GNU General Public License Usage
** 
** This file may be used under the terms of the GNU General Public License 
** version 3.0 as published by the Free Software Foundation and appearing 
** in the file LICENSE.GPL included in the packaging of this file.  Please 
** review the following information to ensure the GNU General Public License 
** version 3.0 requirements will be met: http://www.gnu.org/copyleft/gpl.html.
**
****************************************************************************/

#ifndef QCAMCAPTURETHREAD_HPP
#define QCAMCAPTURETHREAD_HPP

#include <QThread>
#include "QLCVGlobal.hpp"

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

#endif // QCAMCAPTURETHREAD_HPP
