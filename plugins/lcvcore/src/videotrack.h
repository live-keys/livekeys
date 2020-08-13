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
