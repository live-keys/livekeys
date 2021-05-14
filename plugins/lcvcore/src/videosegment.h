#ifndef LVVIDEOSEGMENT_H
#define LVVIDEOSEGMENT_H

#include <QObject>
#include "live/segment.h"
#include "live/qmlstreamfilter.h"

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
    Q_PROPERTY(QString file                       READ file          WRITE setFile    NOTIFY fileChanged)
    Q_PROPERTY(QString filters                    READ filters       WRITE setFilters NOTIFY filtersChanged)
    Q_PROPERTY(lv::QmlStreamFilter* filtersObject READ filtersObject NOTIFY filtersObjectChanged)

public:
    explicit VideoSegment(QObject *parent = nullptr);
    ~VideoSegment() override;

    const QString &file() const;

    void openFile();

    void serialize(QQmlEngine *engine, MLNode &node) const override;
    void deserialize(Track *track, QQmlEngine *engine, const MLNode &data) override;

    void assignTrack(Track *track) override;
    void cursorEnter(qint64 position) override;
    void cursorExit(qint64) override;
    void cursorNext(qint64 position) override;
    void cursorMove(qint64 position) override;

    const QString& filters() const;

    QmlStreamFilter* filtersObject() const;

    static void filtersStreamHandler(QObject* that, const QJSValue& val);
    void streamUpdate(const QJSValue& val);

public slots:
    void setFile(const QString& file);
    void setFilters(const QString& filters);

    virtual void stretchLeftTo(unsigned int position) override;
    virtual void stretchRightTo(unsigned int position) override;

signals:
    void fileChanged();
    void filtersChanged();
    void filtersObjectChanged();

private:
    qint64 segmentToVideoPosition(qint64 segmentPosition);

    void captureFrame(qint64 segmentPosition);
    void frameCaptured(QMat* frame, qint64 position);

    void updateStrechLimits();

    void createFilters();
    void addWatcher();

    VideoTrack*       m_videoTrack;
    QString           m_file;
    cv::VideoCapture* m_capture;
    qint64            m_videoStart;
    qint64            m_videoPosition;
    qint64            m_videoLength;
    QString           m_filters;
    QmlStreamFilter*  m_filtersObject;
    qint64            m_filtersPosition;
};

inline const QString& VideoSegment::file() const{
    return m_file;
}

inline const QString &VideoSegment::filters() const{
    return m_filters;
}

inline QmlStreamFilter *VideoSegment::filtersObject() const{
    return m_filtersObject;
}

inline void VideoSegment::setFile(const QString &file){
    if (m_file == file)
        return;

    m_file = file;
    emit fileChanged();

    openFile();

}

inline void VideoSegment::setFilters(const QString &filters){
    if (m_filters == filters)
        return;

    m_filters = filters;
    emit filtersChanged();

    createFilters();
}

}// namespace

#endif // LVVIDEOSEGMENT_H
