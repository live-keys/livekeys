#include "videosurface.h"
#include <QQmlEngine>

#include "live/stacktrace.h"

namespace lv{

VideoSurface::VideoSurface(QObject*parent)
    : TimelineObjectProperty(parent)
    , m_lastProcessedPosition(-1)
    , m_imageWidth(0)
    , m_imageHeight(0)
    , m_currentPosition(-1)
    , m_buffer(nullptr)
    , m_current(nullptr)
{
}

VideoSurface::~VideoSurface(){
    Shared::unref(m_current);
}

void VideoSurface::swapSurface(qint64 position){
    if ( m_currentPosition != position ){
        m_currentPosition = position;
        if ( !m_buffer ){
            m_buffer = new QMat(m_imageWidth, m_imageHeight, QMat::CV8U, 3);
            m_buffer->internal().setTo(cv::Scalar(0));
        }
        m_current = m_buffer;
        m_buffer = nullptr;
        emit imageChanged();
    }
}

void VideoSurface::updateSurface(qint64 cursorPosition, QMat *frame){
    m_lastProcessedPosition = cursorPosition;
    if ( !m_buffer ){
        m_buffer = new QMat(m_imageWidth, m_imageHeight, QMat::CV8U, 3);
        Shared::ref(m_buffer);
    }

    if ( frame ){
        m_currentPosition = true;
        //TODO: Copy ROI
        frame->internal().copyTo(*m_buffer->internalPtr());
    }
    m_currentPosition = true;
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

}// namespace
