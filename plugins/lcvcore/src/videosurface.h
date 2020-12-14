#ifndef LVVIDEOSURFACE_H
#define LVVIDEOSURFACE_H

#include "qmatdisplay.h"
#include "live/timelineobjectproperty.h"
#include "qvideowriter.h"

namespace lv{

class VideoSurface : public TimelineObjectProperty{

    Q_OBJECT
    Q_PROPERTY(int imageWidth  READ imageWidth  WRITE setImageWidth  NOTIFY imageWidthChanged)
    Q_PROPERTY(int imageHeight READ imageHeight WRITE setImageHeight NOTIFY imageHeightChanged)
    Q_PROPERTY(QMat* image     READ image       NOTIFY imageChanged)

public:
    explicit VideoSurface(QObject *parent = nullptr);
    ~VideoSurface() override;

    void swapSurface(qint64 position);
    void updateSurface(qint64 cursorPosition, QMat* frame);

    virtual void serialize(ViewEngine *engine, MLNode &node) override;
    virtual void deserialize(ViewEngine *engine, const MLNode &node) override;

    int imageWidth() const;
    int imageHeight() const;
    void setImageWidth(int imageWidth);
    void setImageHeight(int imageHeight);

    QMat* image() const;

    qint64 lastProcessedPosition() const;

    bool builderUsed() const;

    void createRecorder();
    void closeRecorder();

public slots:
    void setWriterOptions(const QJSValue& options);

signals:
    void imageWidthChanged();
    void imageHeightChanged();
    void imageChanged();

private:
    qint64        m_lastProcessedPosition;
    int           m_imageWidth;
    int           m_imageHeight;
    qint64        m_currentPosition;
    QMat*         m_buffer;
    QMat*         m_current;
    QJSValue      m_writerOptions;
    QVideoWriter* m_writer;
};

inline int VideoSurface::imageWidth() const{
    return m_imageWidth;
}

inline int VideoSurface::imageHeight() const{
    return m_imageHeight;
}

inline void VideoSurface::setImageWidth(int imageWidth){
    if (m_imageWidth == imageWidth)
        return;

    m_imageWidth = imageWidth;
    emit imageWidthChanged();
}

inline void VideoSurface::setImageHeight(int imageHeight){
    if (m_imageHeight == imageHeight)
        return;

    m_imageHeight = imageHeight;
    emit imageHeightChanged();
}

inline QMat *VideoSurface::image() const{
    return m_current;
}

inline qint64 VideoSurface::lastProcessedPosition() const{
    return m_lastProcessedPosition;
}

inline bool VideoSurface::builderUsed() const{
    return m_currentPosition;
}

}// namespace

#endif // LVVIDEOSURFACE_H
