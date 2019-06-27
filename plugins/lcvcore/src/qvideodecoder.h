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

#ifndef QVIDEODECODER_H
#define QVIDEODECODER_H

#include <QObject>
#include <QQmlEngine>
#include "live/qmlstream.h"

class QVideoDecodeThread;

/// \private
class QVideoDecoder : public QObject{

    Q_OBJECT
    Q_PROPERTY(int     totalFrames  READ totalFrames  NOTIFY totalFramesChanged)
    Q_PROPERTY(QString file         READ file         NOTIFY fileChanged)
    Q_PROPERTY(bool    paused       READ paused       WRITE setPaused       NOTIFY pausedChanged)
    Q_PROPERTY(qreal   fps          READ fps          WRITE setFps          NOTIFY fpsChanged)
    Q_PROPERTY(int     currentFrame READ currentFrame WRITE seekTo          NOTIFY currentFrameChanged)
    Q_PROPERTY(bool    loop         READ loop         WRITE setLoop         NOTIFY loopChanged)

public:
    class Properties{
    public:
        Properties();

        int   totalFrames;
        int   currentFrame;
        qreal fps;
        bool  paused;
        bool  loop;

        bool  isSeeking;
        bool  forceSeek;
    };

public:
    explicit QVideoDecoder(QObject *parent = 0);
    virtual ~QVideoDecoder();

    bool paused() const;
    void setPaused(bool paused);

    qreal fps() const;
    void setFps(qreal fps);

    int totalFrames() const;
    int currentFrame() const;

    QString file() const;

    bool loop() const;
    void setLoop(bool loop);

    qreal runningFps() const;

public slots:
    void switchMat();
    void seekTo(int frame);
    lv::QmlStream* run(const QString& file);

signals:
    void fileChanged();
    void pausedChanged();
    void fpsChanged();
    void currentFrameChanged();
    void totalFramesChanged();
    void loopChanged();

private:
    void initializeMatSize();

    Q_DISABLE_COPY(QVideoDecoder)

    QVideoDecodeThread*        m_worker;
    lv::QmlStream*             m_stream;
    QVideoDecoder::Properties* m_properties;
};

inline qreal QVideoDecoder::fps() const{
    return m_properties->fps;
}

inline bool QVideoDecoder::loop() const{
    return m_properties->loop;
}

inline bool QVideoDecoder::paused() const{
    return m_properties->paused;
}

inline int QVideoDecoder::totalFrames() const{
    return m_properties->totalFrames;
}

inline int QVideoDecoder::currentFrame() const{
    return m_properties->currentFrame;
}

#endif // QVIDEODECODER_H
