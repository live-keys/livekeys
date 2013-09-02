#ifndef QCAMCAPTURETHREAD_HPP
#define QCAMCAPTURETHREAD_HPP

#include <QThread>

class QMat;
class QTimer;
class QCamCaptureThreadPrivate;
class QCamCaptureThread: public QThread{

    Q_OBJECT

public:
    QCamCaptureThread(int device, QObject* parent = 0);
    ~QCamCaptureThread();

    QMat*   output();
    QTimer* timer();
    int     device() const;
    int     captureWidth() const;
    int     captureHeight() const;

    bool    isCaptureOpened();

    void    processNextFrame();

signals:
    void inactiveMatChanged();

public slots:
    void tick();

protected:
    void run();

private:
    void initializeMatSize();

    QCamCaptureThread(const QCamCaptureThread& other);
    QCamCaptureThread& operator= (const QCamCaptureThread& other);

    int     m_deviceId;
    QMat*   m_activeMat;
    QTimer* m_timer;

    QCamCaptureThreadPrivate* const d_ptr;

    Q_DECLARE_PRIVATE(QCamCaptureThread)

};

inline QTimer *QCamCaptureThread::timer(){
    return m_timer;
}

inline int QCamCaptureThread::device() const{
    return m_deviceId;
}

inline QMat *QCamCaptureThread::output(){
    return m_activeMat;
}

#endif // QCAMCAPTURETHREAD_HPP
