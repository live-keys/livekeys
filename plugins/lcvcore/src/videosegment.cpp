#include "videosegment.h"
#include "live/visuallogqt.h"
#include "live/track.h"

#include <QMetaObject>
#include <QJSValue>
#include <QJSValueIterator>

#include "opencv2/core.hpp"
#include "opencv2/highgui.hpp"

namespace lv{

VideoSegment::VideoSegment(QObject *parent)
    : Segment(parent)
    , m_track(nullptr)
    , m_surface(nullptr)
    , m_capture(new cv::VideoCapture)
{
}

void VideoSegment::openFile(){
    if ( m_file.isEmpty() )
        return;

    setLabel(m_file.mid(m_file.lastIndexOf('/') + 1));
    m_capture->open(m_file.toStdString());
}

void VideoSegment::serialize(QQmlEngine *engine, MLNode &node) const{
    Segment::serialize(engine, node);

    node["file"] = m_file.toStdString();
    node["type"] = "VideoSegment";
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

void VideoSegment::deserialize(Track *track, QQmlEngine *engine, const MLNode &node){
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

void VideoSegment::assignTrack(Track *track){
    m_track = track;
    QJSValue val = track->timelineProperties().property("videoCaptureSurface");
    VideoSurface* surface = qobject_cast<VideoSurface*>(val.toQObject());
    if ( surface ){
        setSurface(surface);
    }
}

void VideoSegment::cursorEnter(qint64 pos){
    if ( !m_surface )
        return;

    if ( pos == 0 ){
        if ( m_capture->grab() ){
            cv::Mat* frame = new cv::Mat;
            m_capture->retrieve(*frame);
            QMat* mat = new QMat(frame);
            m_surface->updateSurface(position() + pos, mat);
        }
    } else {
        m_capture->set(cv::CAP_PROP_POS_FRAMES, pos);
        if ( m_capture->grab() ){
            cv::Mat* frame = new cv::Mat;
            m_capture->retrieve(*frame);
            QMat* mat = new QMat(frame);
            m_surface->updateSurface(position() + pos, mat);
        }
    }
}

void VideoSegment::cursorExit(){
    if ( !m_surface )
        return;

    m_surface->output()->cvMat()->setTo(cv::Scalar(0));
    m_surface->update();

    //TODO: Will trigger surface reset
}

void VideoSegment::cursorNext(qint64 pos){
    if ( !m_surface )
        return;

    if ( m_capture->grab() ){
        cv::Mat* frame = new cv::Mat;
        m_capture->retrieve(*frame);
        QMat* mat = new QMat(frame);
        m_surface->updateSurface(position() + pos, mat);
    }
}

void VideoSegment::cursorMove(qint64 position){
    if ( !m_surface )
        return;

    m_capture->set(cv::CAP_PROP_POS_FRAMES, position);
    cv::Mat frame;
    if ( m_capture->grab() ){
        qDebug() << " RETRIEVE: " << m_capture->retrieve(frame);
        frame.copyTo(*m_surface->output()->cvMat());
        m_surface->update();
    }
    qDebug() << "MOVE:" << position;
}

}// namespace
