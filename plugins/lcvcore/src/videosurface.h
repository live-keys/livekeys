#ifndef LVVIDEOSURFACE_H
#define LVVIDEOSURFACE_H

#include "qmatdisplay.h"
#include "live/timelineobjectproperty.h"

namespace lv{

class VideoSurface : public TimelineObjectProperty{

    Q_OBJECT
    Q_PROPERTY(int imageWidth  READ imageWidth  WRITE setImageWidth  NOTIFY imageWidthChanged)
    Q_PROPERTY(int imageHeight READ imageHeight WRITE setImageHeight NOTIFY imageHeightChanged)
    Q_PROPERTY(QMat* image     READ image       WRITE setImage       NOTIFY imageChanged)

public:
    explicit VideoSurface(QObject *parent = nullptr);
    ~VideoSurface() override;

    void resetSurface();
    void updateSurface(qint64 cursorPosition, QMat* frame);

    virtual void serialize(ViewEngine *engine, MLNode &node) override;
    virtual void deserialize(ViewEngine *engine, const MLNode &node) override;

    int imageWidth() const;
    int imageHeight() const;
    void setImageWidth(int imageWidth);
    void setImageHeight(int imageHeight);

    QMat* image() const;
    void setImage(QMat* image);

signals:
    void imageWidthChanged();
    void imageHeightChanged();
    void imageChanged();

private:
    int   m_imageWidth;
    int   m_imageHeight;
    QMat* m_image;
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
    return m_image;
}

}// namespace

#endif // LVVIDEOSURFACE_H
