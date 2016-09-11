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
#ifndef QCAMCAPTURE_H
#define QCAMCAPTURE_H

#include "qlcvglobal.h"
#include "qmatdisplay.h"

class QCamCaptureThread;

class QCamCapture : public QMatDisplay{

    Q_OBJECT
    Q_PROPERTY(QString device     READ device     WRITE setDevice     NOTIFY deviceChanged)
    Q_PROPERTY(bool    paused     READ paused     WRITE setPaused     NOTIFY pausedChanged)
    Q_PROPERTY(qreal   fps        READ fps        WRITE setFps        NOTIFY fpsChanged)
    Q_PROPERTY(QSize   resolution READ resolution WRITE setResolution NOTIFY resolutionChanged)

public:
    explicit QCamCapture(QQuickItem *parent = 0);
    ~QCamCapture();

    const QString& device() const;
    void setDevice(const QString& device);

    bool paused() const;
    void setPaused(bool paused);

    qreal fps() const;
    void setFps(qreal fps);

    const QSize& resolution() const;
    void setResolution(const QSize& resolution);

public slots:
    void switchMat();

signals:
    void deviceChanged();
    void pausedChanged();
    void fpsChanged();
    void resolutionChanged();

private:
    void initializeMatSize();
    void reinitializeDevice();

    QCamCapture(const QCamCapture& other);
    QCamCapture& operator= (const QCamCapture& other);

    QString m_device;
    qreal   m_fps;
    QMat*   m_restore;
    QSize   m_resolution;

    QCamCaptureThread* m_thread;

};

inline const QString &QCamCapture::device() const{
    return m_device;
}

inline qreal QCamCapture::fps() const{
    return m_fps;
}

inline const QSize& QCamCapture::resolution() const{
    return m_resolution;
}

#endif // QCAMCAPTURE_HPP
