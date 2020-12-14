#include "videotrack.h"
#include "videosegment.h"
#include "imagesegment.h"
#include "scriptvideosegment.h"

#include "live/viewcontext.h"
#include "live/project.h"

#include <QQmlPropertyMap>
#include <QQmlEngine>
#include <QQmlContext>
#include <QFileInfo>

namespace lv{

VideoTrack::VideoTrack(QObject *parent)
    : Track(parent)
    , m_surface(nullptr)
{
}

VideoTrack::~VideoTrack(){

}

void VideoTrack::serialize(ViewEngine *viewEngine, MLNode &node) const{
    Track::serialize(viewEngine, node);
    node["type"] = "lcvcore#VideoTrack";
    node["factory"] = "lcvcore/VideoTrackFactory.qml";
}

void VideoTrack::deserialize(ViewEngine *engine, const MLNode &node){
    Track::deserialize(engine, node);

    const MLNode::ArrayType& segments = node["segments"].asArray();
    for ( auto it = segments.begin(); it != segments.end(); ++it ){
        const MLNode& segmNode = *it;
        if ( segmNode["type"].asString() == "VideoSegment" ){
            VideoSegment* segment = new VideoSegment;
            segment->deserialize(this, engine->engine(), segmNode);
            addSegment(segment);
        } else if ( segmNode["type"].asString() == "ImageSegment" ){
            ImageSegment* segment = new ImageSegment;
            segment->deserialize(this, engine->engine(), segmNode);
            addSegment(segment);
        } else if ( segmNode["type"].asString() == "ScriptVideoSegment" ){
            QString scriptPath = QString::fromStdString(segmNode["script"].asString());
            QFileInfo finfo(scriptPath);
            if ( finfo.isRelative() ){
                QObject* projectOb = engine->engine()->rootContext()->contextProperty("project").value<QObject*>();
                Project* project = qobject_cast<Project*>(projectOb);
                if ( project ){
                    scriptPath = QFileInfo(project->dir() + "/" + scriptPath).canonicalFilePath();
                }
            }
            ViewEngine::ComponentResult::Ptr cr = engine->createObject(scriptPath, nullptr);
            if ( cr->hasError() ){
                cr->jsThrowError();
                return;
            }
            ScriptVideoSegment* segment = qobject_cast<ScriptVideoSegment*>(cr->object);
            segment->deserialize(this, engine->engine(), segmNode);
            addSegment(segment);
        } else {
            THROW_QMLERROR(Exception, "VideoTrack: Unknown segment type: " + segmNode["type"].asString(), Exception::toCode("~Segment"), this);
            return;
        }
    }
}

void VideoTrack::timelineComplete(){
    QObject* tp = timelineProperties();
    if ( !tp ){
        THROW_QMLERROR(Exception, "VideoTrack: Failed to fetch timeline properties", Exception::toCode("~Timeline"), this);
        return;
    }

    QObject* surf = tp->property("videoSurface").value<QObject*>();
    if ( !surf ){
        THROW_QMLERROR(Exception, "VideoTrack: Timeline does not have a video surface assigned", Exception::toCode("~VideoSurface"), this);
        return;
    }

    m_surface = qobject_cast<VideoSurface*>(surf);

    Track::timelineComplete();
}

void VideoTrack::cursorPositionProcessed(qint64 position){
    Track::cursorPositionProcessed(position);
    if ( surface() ){
        surface()->swapSurface(position);
    }
}

void VideoTrack::recordingStarted(){
    try {
        m_surface->createRecorder();
    } catch (lv::Exception& e) {
        ViewEngine* ve = ViewContext::instance().engine();
        QmlError(ve, e, this).jsThrow();
    }
}

void VideoTrack::recordingStopped(){
    m_surface->closeRecorder();
}


}// namespace
