#ifndef QCAMCAPTURE_HPP
#define QCAMCAPTURE_HPP

#include "QMatDisplay.hpp"

class QCamCapturePrivate;
class QCamCapture : public QMatDisplay{

    Q_OBJECT
    Q_PROPERTY(int  device READ device WRITE setDevice NOTIFY deviceChanged)
    Q_PROPERTY(bool paused READ paused WRITE setPaused NOTIFY pausedChanged)

public:
    explicit QCamCapture(QQuickItem *parent = 0);
    ~QCamCapture();

    int device() const;
    void setDevice(int device);

    bool paused() const;
    void setPaused(bool paused);

signals:
    void deviceChanged();
    void pausedChanged();

private:
    int  m_device;
    bool m_paused;

    QCamCapturePrivate* d_ptr;

};

inline int QCamCapture::device() const{
    return m_device;
}

inline bool QCamCapture::paused() const{
    return m_paused;
}

#endif // QCAMCAPTURE_HPP
