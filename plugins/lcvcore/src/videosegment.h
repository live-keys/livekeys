#ifndef LVVIDEOSEGMENT_H
#define LVVIDEOSEGMENT_H

#include <QObject>
#include "live/segment.h"

#include "qvideocapture.h"
#include "videosurface.h"

namespace cv{
class VideoCapture;
}

namespace lv{

/// \private
class VideoSegment : public Segment{

    Q_OBJECT
    Q_PROPERTY(lv::VideoSurface* surface READ surface WRITE setSurface NOTIFY surfaceChanged)
    Q_PROPERTY(QString file  READ file    WRITE setFile    NOTIFY fileChanged)

public:
    explicit VideoSegment(QObject *parent = nullptr);

    VideoSurface* surface() const;
    void setSurface(VideoSurface* surface);

    const QString &file() const;

    void openFile();

    void serialize(QQmlEngine *engine, MLNode &node) const override;
    void deserialize(Track *track, QQmlEngine *engine, const MLNode &data) override;

    void assignTrack(Track *track) override;
    void cursorEnter(qint64 position) override;
    void cursorExit() override;
    void cursorNext(qint64 position) override;
    void cursorMove(qint64 position) override;

public slots:
    void setFile(const QString& file);

signals:
    void surfaceChanged();
    void fileChanged();

private:
    Track*        m_track;
    VideoSurface* m_surface;
    QString       m_file;
    cv::VideoCapture* m_capture;
};

inline VideoSurface *VideoSegment::surface() const{
    return m_surface;
}

inline void VideoSegment::setSurface(VideoSurface *surface){
    if (m_surface == surface)
        return;

    m_surface = surface;
    emit surfaceChanged();
}

inline const QString& VideoSegment::file() const{
    return m_file;
}

inline void VideoSegment::setFile(const QString &file){
    if (m_file == file)
        return;

    m_file = file;
    emit fileChanged();

    openFile();

}

}// namespace

#endif // LVVIDEOSEGMENT_H
