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
#ifndef QCAMCAPTURE_HPP
#define QCAMCAPTURE_HPP

#include "QMatDisplay.hpp"
#include "QLCVGlobal.hpp"

class QCamCaptureThread;

class QCamCapture : public QMatDisplay{

    Q_OBJECT
    Q_PROPERTY(QString device READ device WRITE setDevice NOTIFY deviceChanged)
    Q_PROPERTY(bool    paused READ paused WRITE setPaused NOTIFY pausedChanged)
    Q_PROPERTY(qreal   fps    READ fps    WRITE setFps    NOTIFY fpsChanged)

public:
    explicit QCamCapture(QQuickItem *parent = 0);
    ~QCamCapture();

    const QString& device() const;
    void setDevice(const QString& device);

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

    QString m_device;
    qreal   m_fps;
    QMat*   m_restore;

    QCamCaptureThread* m_thread;

};

inline const QString &QCamCapture::device() const{
    return m_device;
}

inline qreal QCamCapture::fps() const{
    return m_fps;
}

#endif // QCAMCAPTURE_HPP
