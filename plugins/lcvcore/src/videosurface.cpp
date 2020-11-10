#include "videosurface.h"
#include <QQmlEngine>

#include "live/stacktrace.h"


namespace lv{

VideoSurface::VideoSurface(QObject*parent)
    : TimelineObjectProperty(parent)
    , m_imageWidth(0)
    , m_imageHeight(0)
    , m_image(nullptr)
{
}

VideoSurface::~VideoSurface(){
    Shared::unref(m_image);
}

void VideoSurface::resetSurface(){
    if ( m_image ){
        m_image->internalPtr()->setTo(cv::Scalar(0));
        emit imageChanged();
    }
}

void VideoSurface::updateSurface(qint64 /*cursorPosition*/, QMat *frame){
    setImage(frame);
}

void VideoSurface::serialize(ViewEngine *, MLNode &node){
    node["width"]   = m_imageWidth;
    node["height"]  = m_imageHeight;
    node["type"]    = "lcvcore#VideoSurface";
    node["factory"] = "lcvcore/VideoSurfaceFactory.qml";
}

void VideoSurface::deserialize(ViewEngine *, const MLNode &node){
    m_imageWidth  = node["width"].asInt();
    m_imageHeight = node["height"].asInt();
}

void lv::VideoSurface::setImage(QMat *image){
    if (m_image == image)
        return;

    Shared::unref(m_image);

    m_image = image;
    Shared::ref(m_image);
    emit imageChanged();
}

}// namespace
