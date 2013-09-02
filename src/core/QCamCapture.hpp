#ifndef QCAMCAPTURE_HPP
#define QCAMCAPTURE_HPP

#include "QMatDisplay.hpp"

class QCamCaptureThread;
class QCamCapture : public QMatDisplay{

    Q_OBJECT
    Q_PROPERTY(int    device READ device WRITE setDevice NOTIFY deviceChanged)
    Q_PROPERTY(bool   paused READ paused WRITE setPaused NOTIFY pausedChanged)
    Q_PROPERTY(qreal  fps    READ fps    WRITE setFps    NOTIFY fpsChanged)

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

    QCamCapture(const QCamCapture& other);
    QCamCapture& operator= (const QCamCapture& other);

    int   m_device;
    bool  m_paused;
    qreal m_fps;
	QMat* m_restore;

    QCamCaptureThread* m_thread;

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
