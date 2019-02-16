/****************************************************************************
**
** Copyright (C) 2014-2018 Dinu SV.
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

#include "qmatdisplay.h"
class QCamCaptureThread;
/// \private
class QCamCapture : public QMatDisplay{

    Q_OBJECT
    Q_PROPERTY(QString device     READ device     NOTIFY deviceChanged)
    Q_PROPERTY(QSize   resolution READ resolution NOTIFY resolutionChanged)
    Q_PROPERTY(bool    paused     READ paused     WRITE setPaused     NOTIFY pausedChanged)
    Q_PROPERTY(qreal   fps        READ fps        WRITE setFps        NOTIFY fpsChanged)

public:
    explicit QCamCapture(QQuickItem *parent = 0);
    ~QCamCapture();

    const QString& device() const;

    bool paused() const;
    void setPaused(bool paused);

    qreal fps() const;
    void setFps(qreal fps);

    const QSize& resolution() const;

public slots:
    void switchMat();
    void staticOpen(const QString& device, const QSize& resolution = QSize());
    void staticLoad(const QString& device, const QSize& resolution = QSize());

signals:
    void deviceChanged();
    void pausedChanged();
    void fpsChanged();
    void resolutionChanged();
    void init();

protected:
    virtual void componentComplete();

private:
    void initializeMatSize();

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

inline void QCamCapture::staticOpen(const QString &device, const QSize &resolution){
    staticLoad(device, resolution);
}

#endif // QCAMCAPTURE_H
