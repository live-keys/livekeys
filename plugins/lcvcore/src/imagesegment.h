#ifndef LVIMAGESEGMENT_H
#define LVIMAGESEGMENT_H

#include <QObject>
#include "live/segment.h"
#include "live/qmlstreamfilter.h"

#include "videosurface.h"
#include "videotrack.h"

namespace lv{

class ImageSegment : public Segment{

    Q_OBJECT
    Q_PROPERTY(QString file                       READ file          WRITE setFile    NOTIFY fileChanged)
    Q_PROPERTY(QString filters                    READ filters       WRITE setFilters NOTIFY filtersChanged)
    Q_PROPERTY(lv::QmlStreamFilter* filtersObject READ filtersObject NOTIFY filtersObjectChanged)

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

    const QString& filters() const;
    void setFilters(const QString &filters);

    QmlStreamFilter* filtersObject() const;

    static void filtersStreamHandler(QObject* that, const QJSValue& val);
    void streamUpdate(const QJSValue& val);

signals:
    void surfaceChanged();
    void imageChanged();
    void fileChanged();
    void filtersChanged();
    void filtersObjectChanged();

private:
    void frameCaptured(QMat* frame, qint64 position);
    void createFilters();
    void addWatcher();

    VideoTrack*          m_videoTrack;
    QString              m_file;
    QMat*                m_image;
    QString              m_filters;
    lv::QmlStreamFilter* m_filtersObject;
    qint64               m_filtersPosition;
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

inline const QString &ImageSegment::filters() const{
    return m_filters;
}

inline void ImageSegment::setFilters(const QString& filters){
    if (m_filters == filters)
        return;

    m_filters = filters;
    emit filtersChanged();

    createFilters();
}

inline QmlStreamFilter *ImageSegment::filtersObject() const{
    return m_filtersObject;
}

}// namespace

#endif // IMAGESEGMENT_H
