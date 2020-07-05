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
    virtual ~VideoTrack();

    VideoSurface* surface() const;
    void setSurface(lv::VideoSurface* surface);

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

}// namespace

#endif // VIDEOTRACK_H
