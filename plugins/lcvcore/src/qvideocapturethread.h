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

#ifndef QVIDEOCAPTURETHREAD_H
#define QVIDEOCAPTURETHREAD_H

#include <QThread>

class QMat;
class QTimer;
class QVideoCaptureThreadPrivate;
class QVideoCaptureThread: public QThread{

    Q_OBJECT

public:
    QVideoCaptureThread(const QString& file, QObject* parent = 0);
    ~QVideoCaptureThread();

    QMat*   output();
    QTimer* timer();
    const QString& file() const;
    int     captureWidth() const;
    int     captureHeight() const;
    double  captureFps() const;

    bool    paused() const;
    void    setPaused(bool paused);

    bool    isSeeking() const;
    void    setForceSeek(bool forceSeek);

    int     currentFrame() const;
    int     totalFrames() const;
    bool    isCaptureOpened();

    void    processNextFrame();

    void    setLoop(bool loop);

signals:
    void inactiveMatChanged();
    void isSeekingChanged();

public slots:
    void tick();
    void seekTo(int frame);

protected:
    void run();

private:
    void initializeMatSize();

    void beginSeek();
    void endSeek();

    QVideoCaptureThread(const QVideoCaptureThread& other);
    QVideoCaptureThread& operator= (const QVideoCaptureThread& other);

    QString m_file;
    bool    m_paused;
    int     m_framePos;
    int     m_totalFrames;
    bool    m_isSeeking;
    bool    m_forceSeek;
    bool    m_loop;
    QMat*   m_activeMat;
    QTimer* m_timer;

    QVideoCaptureThreadPrivate* const d_ptr;

    Q_DECLARE_PRIVATE(QVideoCaptureThread)

};

inline QTimer *QVideoCaptureThread::timer(){
    return m_timer;
}

inline const QString& QVideoCaptureThread::file() const{
    return m_file;
}

inline bool QVideoCaptureThread::paused() const{
    return m_paused;
}

inline void QVideoCaptureThread::setPaused(bool paused){
    m_paused = paused;
}

inline bool QVideoCaptureThread::isSeeking() const{
    return m_isSeeking;
}

inline int QVideoCaptureThread::currentFrame() const{
    return m_framePos;
}

inline int QVideoCaptureThread::totalFrames() const{
    return m_totalFrames;
}

inline void QVideoCaptureThread::setForceSeek(bool forceSeek){
    m_forceSeek = forceSeek;
}

inline QMat *QVideoCaptureThread::output(){
    return m_activeMat;
}

#endif // QVIDEOCAPTURETHREAD_H
