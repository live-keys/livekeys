#ifndef LVVIDEOSEGMENT_H
#define LVVIDEOSEGMENT_H

#include <QObject>
#include "live/segment.h"

#include "qvideocapture.h"
#include "videosurface.h"
#include "videotrack.h"

namespace cv{
class VideoCapture;
}

namespace lv{

/// \private
class VideoSegment : public Segment{

    Q_OBJECT
    Q_PROPERTY(QString file  READ file WRITE setFile NOTIFY fileChanged)

public:
    explicit VideoSegment(QObject *parent = nullptr);

    const QString &file() const;

    void openFile();

    void serialize(QQmlEngine *engine, MLNode &node) const override;
    void deserialize(Track *track, QQmlEngine *engine, const MLNode &data) override;

    void assignTrack(Track *track) override;
    void cursorEnter(qint64 position) override;
    void cursorExit(qint64) override;
    void cursorNext(qint64 position) override;
    void cursorMove(qint64 position) override;

public slots:
    void setFile(const QString& file);

signals:
    void fileChanged();

private:
    VideoTrack*       m_videoTrack;
    QString           m_file;
    cv::VideoCapture* m_capture;
};

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
