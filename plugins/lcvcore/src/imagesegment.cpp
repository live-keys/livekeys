#include "imagesegment.h"

#include "live/mlnode.h"
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

    qDebug() << m_image;
}

void ImageSegment::serialize(QQmlEngine *engine, MLNode &node) const{
    Segment::serialize(engine, node);

    node["file"] = m_file.toStdString();
    node["type"] = "ImageSegment";
    node["factory"] = "lcvcore/ImageSegmentFactory.qml";
}

void ImageSegment::deserialize(Track *track, QQmlEngine *engine, const MLNode &node){
    Segment::deserialize(track, engine, node);
    setFile(QString::fromStdString(node["file"].asString()));
}

void ImageSegment::assignTrack(Track *track){
    VideoTrack* nt = qobject_cast<VideoTrack*>(track);
    if ( !nt ){
        Exception e = CREATE_EXCEPTION(lv::Exception, "NumberAnimationSegment needs NumberTrack at '" + track->name().toStdString() + "'.", Exception::toCode("~Track") );
        lv::ViewContext::instance().engine()->throwError(&e, this);
        return;
    }
    m_track = nt;
}

void ImageSegment::cursorEnter(qint64 pos){
    if ( !m_track || !m_track->surface() || !m_image)
        return;

    m_track->surface()->updateSurface(position() + pos, m_image->cloneMat());
}

void ImageSegment::cursorExit(qint64){
    if ( !m_track || !m_track->surface() || !m_image)
        return;

    m_track->surface()->output()->cvMat()->setTo(cv::Scalar(0));
    m_track->surface()->update();
}

void ImageSegment::cursorNext(qint64 pos){
    if ( !m_track || !m_track->surface() || !m_image)
        return;

    m_track->surface()->updateSurface(position() + pos, m_image->cloneMat());
}

void ImageSegment::cursorMove(qint64 pos){
    if ( !m_track || !m_track->surface() || !m_image)
        return;

    m_track->surface()->updateSurface(position() + pos, m_image->cloneMat());
}

}// namespace
