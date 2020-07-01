#include "imagesegment.h"
#include "live/mlnode.h"
#include "live/track.h"

#include <QMetaObject>
#include <QJSValue>
#include <QJSValueIterator>

#include "opencv2/core.hpp"
#include "opencv2/highgui.hpp"

namespace lv{

ImageSegment::ImageSegment(QObject *parent)
    : Segment(parent)
    , m_track(nullptr)
    , m_image(new QMat)
{
}

ImageSegment::~ImageSegment(){
    delete m_image;
}

void ImageSegment::openFile(){
    if ( m_file.isEmpty() )
        return;

    setLabel(m_file.mid(m_file.lastIndexOf('/') + 1));
    *m_image->cvMat() = cv::imread(m_file.toStdString());
}

void ImageSegment::serialize(QQmlEngine *engine, MLNode &node) const{
    Segment::serialize(engine, node);

    node["file"] = m_file.toStdString();
    node["type"] = "ImageSegment";
    node["factory"] = "lcvcore/VideoCaptureSegmentFactory.qml";

    if ( m_surface && m_track ){
        QJSValue properties = m_track->timelineProperties();

        QJSValueIterator pit(properties);
        while ( pit.next() ){
            QJSValue current = pit.value();
            if ( current.isQObject() ){
                QObject* obj = current.toQObject();
                if ( obj == m_surface ){
                    node["surface"] = pit.name().toStdString();
                }
            }
        }
    }

}

void ImageSegment::deserialize(Track *track, QQmlEngine *engine, const MLNode &node){
    Segment::deserialize(track, engine, node);

    setFile(QString::fromStdString(node["file"].asString()));
    if ( node.hasKey("surface") ){
        QJSValue properties = track->timelineProperties();
        QJSValue surface = properties.property(QString::fromStdString(node["surface"].asString()));
        if ( surface.isQObject() ){
            m_surface = qobject_cast<VideoSurface*>(surface.toQObject());
        }
    }
}

void ImageSegment::assignTrack(Track *track){
    m_track = track;
    QJSValue val = track->timelineProperties().property("videoCaptureSurface");
    VideoSurface* surface = qobject_cast<VideoSurface*>(val.toQObject());
    if ( surface ){
        setSurface(surface);
    }
}

void ImageSegment::cursorEnter(qint64 position){
    if ( !m_surface || !m_image)
        return;

    m_surface->updateSurface(position, m_image->cloneMat());
}

void ImageSegment::cursorExit(){
    if ( !m_surface || !m_image)
        return;

    m_surface->output()->cvMat()->setTo(cv::Scalar(0));
    m_surface->update();
}

void ImageSegment::cursorNext(qint64 position){
    if ( !m_surface || !m_image)
        return;

    m_surface->updateSurface(position, m_image->cloneMat());
}

void ImageSegment::cursorMove(qint64 position){
    if ( !m_surface || !m_image)
        return;

    m_surface->updateSurface(position, m_image->cloneMat());
}

}// namespace
