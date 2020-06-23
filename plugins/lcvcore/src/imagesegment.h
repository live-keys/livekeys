#ifndef LVIMAGESEGMENT_H
#define LVIMAGESEGMENT_H

#include <QObject>
#include "live/segment.h"
#include "videosurface.h"

namespace lv{

class ImageSegment : public Segment{

    Q_OBJECT
    Q_PROPERTY(lv::VideoSurface* surface READ surface WRITE setSurface NOTIFY surfaceChanged)
    Q_PROPERTY(QString file              READ file    WRITE setFile    NOTIFY fileChanged)

public:
    explicit ImageSegment(QObject *parent = nullptr);
    ~ImageSegment() override;

    void openFile();

    VideoSurface *surface() const;
    void setSurface(VideoSurface* surface);

    const QString& file() const;
    void setFile(const QString &file);

    void serialize(QQmlEngine *engine, MLNode &node) const override;
    void deserialize(Track *track, QQmlEngine *engine, const MLNode &data) override;

    void assignTrack(Track *track) override;
    void cursorEnter(qint64 position) override;
    void cursorExit() override;
    void cursorNext(qint64 position) override;
    void cursorMove(qint64 position) override;

signals:
    void surfaceChanged();
    void imageChanged();
    void fileChanged();

private:
    Track*            m_track;
    QString           m_file;
    lv::VideoSurface* m_surface;
    QMat*             m_image;
};

inline void ImageSegment::setSurface(VideoSurface* surface){
    if (m_surface == surface)
        return;

    m_surface = surface;
    emit surfaceChanged();
}

inline VideoSurface* ImageSegment::surface() const{
    return m_surface;
}

inline const QString& ImageSegment::file() const{
    return m_file;
}

inline void ImageSegment::setFile(const QString& file){
    if (m_file == file)
        return;

    m_file = file;
    emit fileChanged();

    openFile();
}

}// namespace

#endif // IMAGESEGMENT_H
