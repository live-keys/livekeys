#include "imagesegment.h"

#include "live/mlnode.h"
#include "live/track.h"
#include "live/viewcontext.h"
#include "live/viewengine.h"
#include "live/exception.h"
#include "live/project.h"

#include <QMetaObject>
#include <QJSValue>
#include <QJSValueIterator>
#include <QQmlContext>
#include <QQmlEngine>
#include <QFileInfo>

#include "opencv2/core.hpp"
#include "opencv2/highgui.hpp"

namespace lv{

ImageSegment::ImageSegment(QObject *parent)
    : Segment(parent)
    , m_videoTrack(nullptr)
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
    *m_image->internalPtr() = cv::imread(m_file.toStdString());
}

void ImageSegment::serialize(QQmlEngine *engine, MLNode &node) const{
    Segment::serialize(engine, node);

    QString filePath = m_file;

    QObject* projectOb = engine->rootContext()->contextProperty("project").value<QObject*>();
    Project* project = qobject_cast<Project*>(projectOb);

    if ( project ){
        if ( filePath.startsWith(project->dir()) ){
            filePath = filePath.mid(project->dir().length());
        }
    }

    node["file"] = filePath.toStdString();
    node["type"] = "ImageSegment";
    node["factory"] = "lcvcore/ImageSegmentFactory.qml";
}

void ImageSegment::deserialize(Track *track, QQmlEngine *engine, const MLNode &node){
    Segment::deserialize(track, engine, node);

    QString fileName = QString::fromStdString(node["file"].asString());
    QFileInfo finfo(fileName);
    if ( finfo.isRelative() ){
        QObject* projectOb = engine->rootContext()->contextProperty("project").value<QObject*>();
        Project* project = qobject_cast<Project*>(projectOb);
        if ( project ){
            fileName = QFileInfo(project->dir() + "/" + fileName).canonicalFilePath();
        }
    }

    setFile(fileName);
}

void ImageSegment::assignTrack(Track *track){
    VideoTrack* nt = qobject_cast<VideoTrack*>(track);
    if ( !nt ){
        Exception e = CREATE_EXCEPTION(lv::Exception, "NumberAnimationSegment needs NumberTrack at '" + track->name().toStdString() + "'.", Exception::toCode("~Track") );
        lv::ViewContext::instance().engine()->throwError(&e, this);
        return;
    }
    m_videoTrack = nt;
    Segment::assignTrack(track);
}

void ImageSegment::cursorEnter(qint64 pos){
    if ( !m_videoTrack || !m_videoTrack->surface() || !m_image)
        return;

    m_videoTrack->surface()->updateSurface(position() + pos, m_image->cloneMat());
}

void ImageSegment::cursorExit(qint64){
    if ( !m_videoTrack || !m_videoTrack->surface() || !m_image)
        return;
}

void ImageSegment::cursorNext(qint64 pos){
    if ( !m_videoTrack || !m_videoTrack->surface() || !m_image)
        return;

    m_videoTrack->surface()->updateSurface(position() + pos, m_image->cloneMat());
}

void ImageSegment::cursorMove(qint64 pos){
    if ( !m_videoTrack || !m_videoTrack->surface() || !m_image)
        return;

    m_videoTrack->surface()->updateSurface(position() + pos, m_image->cloneMat());
}

}// namespace
