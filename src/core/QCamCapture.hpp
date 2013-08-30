#ifndef QCAMCAPTURE_HPP
#define QCAMCAPTURE_HPP

#include "QMatDisplay.hpp"

class QCamCaptureThread;


#include "opencv2/video/video.hpp"
#include "opencv2/highgui/highgui.hpp"

#include <QThread>
#include <QTimer>
#include <QReadWriteLock>
#include <QWaitCondition>

using namespace cv;

class QCamCaptureThread: public QThread{

    Q_OBJECT

public:
    explicit QCamCaptureThread(QObject* parent = 0);
    ~QCamCaptureThread();

    VideoCapture*  capture;
    QMat*          inactiveMat;
    QTimer         timer;

    bool           inactiveMatReady;
    bool           m_abord;

    int            m_deviceId;

    QMutex         m_mutex;
    QWaitCondition m_condition;

signals:
    void inactiveMatChanged();

    // inactive mat set/get put release lock and wait function ( similar example )

public slots:
    void tick();

protected:
    void run();

};

class QCamCapture : public QMatDisplay{

    Q_OBJECT
    Q_PROPERTY(int    device READ device WRITE setDevice NOTIFY deviceChanged)
    Q_PROPERTY(bool   paused READ paused WRITE setPaused NOTIFY pausedChanged)
    Q_PROPERTY(qreal  fps    READ fps    WRITE setFps    NOTIFY fpsChanged)

    friend class QCamCaptureThread;

public:
    explicit QCamCapture(QQuickItem *parent = 0);
    ~QCamCapture();

    int device() const;
    void setDevice(int device);

    bool paused() const;
    void setPaused(bool paused);

    qreal fps() const;
    void setFps(qreal fps);

public slots:
    void switchMat();

signals:
    void deviceChanged();
    void pausedChanged();
    void fpsChanged();

private:
    void initializeMatSize();


    int   m_device;
    bool  m_paused;
    qreal m_fps;

    QCamCaptureThread* d_ptr;

};

inline int QCamCapture::device() const{
    return m_device;
}

inline bool QCamCapture::paused() const{
    return m_paused;
}

inline qreal QCamCapture::fps() const{
    return m_fps;
}

#endif // QCAMCAPTURE_HPP
