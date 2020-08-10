#include "videosegment.h"
#include "live/visuallogqt.h"
#include "live/track.h"

#include "live/viewcontext.h"
#include "live/viewengine.h"
#include "live/exception.h"

#include <QMetaObject>
#include <QJSValue>
#include <QJSValueIterator>

#include "opencv2/core.hpp"
#include "opencv2/highgui.hpp"

namespace lv{

VideoSegment::VideoSegment(QObject *parent)
    : Segment(parent)
    , m_track(nullptr)
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
}

void VideoSegment::deserialize(Track *track, QQmlEngine *engine, const MLNode &node){
    Segment::deserialize(track, engine, node);

    setFile(QString::fromStdString(node["file"].asString()));
}

void VideoSegment::assignTrack(Track *track){
    VideoTrack* nt = qobject_cast<VideoTrack*>(track);
    if ( !nt ){
        Exception e = CREATE_EXCEPTION(lv::Exception, "NumberAnimationSegment needs NumberTrack at '" + track->name().toStdString() + "'.", Exception::toCode("~Track") );
        lv::ViewContext::instance().engine()->throwError(&e, this);
        return;
    }
    m_track = nt;
}

void VideoSegment::cursorEnter(qint64 pos){
    if ( !m_track || !m_track->surface() )
        return;

    if ( pos == 0 ){
        if ( m_capture->grab() ){
            cv::Mat* frame = new cv::Mat;
            m_capture->retrieve(*frame);
            QMat* mat = new QMat(frame);
            m_track->surface()->updateSurface(position() + pos, mat);
        }
    } else {
        m_capture->set(cv::CAP_PROP_POS_FRAMES, pos);
        if ( m_capture->grab() ){
            cv::Mat* frame = new cv::Mat;
            m_capture->retrieve(*frame);
            QMat* mat = new QMat(frame);
            m_track->surface()->updateSurface(position() + pos, mat);
        }
    }
}

void VideoSegment::cursorExit(qint64){
    if ( !m_track || !m_track->surface() )
        return;

    m_track->surface()->resetSurface();
}

void VideoSegment::cursorNext(qint64 pos){
    if ( !m_track || !m_track->surface() )
        return;

    if ( m_capture->grab() ){
        cv::Mat* frame = new cv::Mat;
        m_capture->retrieve(*frame);
        QMat* mat = new QMat(frame);
        m_track->surface()->updateSurface(position() + pos, mat);
    }
}

void VideoSegment::cursorMove(qint64 pos){
    if ( !m_track || !m_track->surface() )
        return;

    m_capture->set(cv::CAP_PROP_POS_FRAMES, pos);
    cv::Mat frame;
    if ( m_capture->grab() ){
        cv::Mat* frame = new cv::Mat;
        m_capture->retrieve(*frame);
        QMat* mat = new QMat(frame);
        m_track->surface()->updateSurface(position() + pos, mat);
    }
}

}// namespace
