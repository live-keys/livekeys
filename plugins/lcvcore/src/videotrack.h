/****************************************************************************
**
** Copyright (C) 2022 Dinu SV.
** This file is part of Livekeys Application.
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

#ifndef VIDEOTRACK_H
#define VIDEOTRACK_H

#include <QObject>
#include "live/track.h"
#include "videosurface.h"

namespace lv{

class VideoTrack : public Track{

    Q_OBJECT
    Q_PROPERTY(lv::VideoSurface* surface READ surface WRITE setSurface NOTIFY surfaceChanged)

public:
    explicit VideoTrack(QObject *parent = nullptr);
    virtual ~VideoTrack() override;

    VideoSurface* surface() const;
    void setSurface(lv::VideoSurface* surface);

    virtual void serialize(ViewEngine *engine, MLNode &node) const override;
    virtual void deserialize(ViewEngine *engine, const MLNode &node) override;

    virtual void timelineComplete() override;
    virtual QString typeReference() const override;

    virtual void cursorPositionProcessed(qint64 position) override;
    virtual void recordingStarted() override;
    virtual void recordingStopped() override;

public slots:
    virtual QJSValue configuredProperties(lv::Segment *segment) const override;

signals:
    void surfaceChanged(lv::VideoSurface* surface);

private:
    VideoSurface* m_surface;
};

inline VideoSurface *VideoTrack::surface() const{
    return m_surface;
}

inline void VideoTrack::setSurface(VideoSurface *surface){
    if (m_surface == surface)
        return;

    m_surface = surface;
    emit surfaceChanged(m_surface);
}

inline QString VideoTrack::typeReference() const{
    return "lcvcore#VideoTrack";
}

}// namespace

#endif // VIDEOTRACK_H
