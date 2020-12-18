#ifndef LVIMAGESEGMENT_H
#define LVIMAGESEGMENT_H

#include <QObject>
#include "live/segment.h"
#include "videosurface.h"
#include "videotrack.h"

namespace lv{

class ImageSegment : public Segment{

    Q_OBJECT
    Q_PROPERTY(QString file  READ file    WRITE setFile    NOTIFY fileChanged)

public:
    explicit ImageSegment(QObject *parent = nullptr);
    ~ImageSegment() override;

    void openFile();

    const QString& file() const;
    void setFile(const QString &file);

    void serialize(QQmlEngine *engine, MLNode &node) const override;
    void deserialize(Track *track, QQmlEngine *engine, const MLNode &data) override;

    void assignTrack(Track *track) override;
    void cursorEnter(qint64 position) override;
    void cursorExit(qint64) override;
    void cursorNext(qint64 position) override;
    void cursorMove(qint64 position) override;

signals:
    void surfaceChanged();
    void imageChanged();
    void fileChanged();

private:
    VideoTrack*       m_videoTrack;
    QString           m_file;
    QMat*             m_image;
};

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
