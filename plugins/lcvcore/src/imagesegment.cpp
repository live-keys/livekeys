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

#include "live/hookcontainer.h"
#include "live/qmlwatcher.h"

namespace lv{

ImageSegment::ImageSegment(QObject *parent)
    : Segment(parent)
    , m_videoTrack(nullptr)
    , m_image(new QMat)
    , m_filtersObject(nullptr)
    , m_filtersPosition(-1)
{
}

ImageSegment::~ImageSegment(){
    delete m_image;
    delete m_filtersObject;
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
    QString filtersPath = m_filters;

    QObject* projectOb = engine->rootContext()->contextProperty("project").value<QObject*>();
    Project* project = qobject_cast<Project*>(projectOb);

    if ( project ){
        if ( filePath.startsWith(project->dir()) ){
            filePath = filePath.mid(project->dir().length());
        }

        if ( filtersPath.startsWith(project->dir() ) ){
            filtersPath = filtersPath.mid(project->dir().length() + 1);
        }
    }
    if ( !m_filters.isEmpty() ){
        node["filters"] = filtersPath.toStdString();
    }

    node["file"] = filePath.toStdString();
    node["type"] = "ImageSegment";
    node["factory"] = "lcvcore/ImageSegmentFactory.qml";
}

void ImageSegment::deserialize(Track *track, QQmlEngine *engine, const MLNode &node){
    Segment::deserialize(track, engine, node);

    QString fileName = QString::fromStdString(node["file"].asString());
    QFileInfo finfo(fileName);


    QObject* projectOb = engine->rootContext()->contextProperty("project").value<QObject*>();
    Project* project = qobject_cast<Project*>(projectOb);

    if ( finfo.isRelative() && project ){
        fileName = QFileInfo(project->dir() + "/" + fileName).canonicalFilePath();
    }

    if ( node.hasKey("filters") ){
        QString filtersPath = QString::fromStdString(node["filters"].asString());
        QFileInfo filterPathInfo(filtersPath);
        if ( filterPathInfo.isRelative() ){
            filtersPath = QFileInfo(project->dir() + "/" + filtersPath).canonicalFilePath();
        }
        setFilters(filtersPath);
    }


    setFile(fileName);
}

void ImageSegment::assignTrack(Track *track){
    VideoTrack* nt = qobject_cast<VideoTrack*>(track);
    if ( !nt ){
        Exception e = CREATE_EXCEPTION(lv::Exception, "ImageSegment needs NumberTrack at '" + track->name().toStdString() + "'.", Exception::toCode("~Track") );
        lv::ViewContext::instance().engine()->throwError(&e, this);
        return;
    }
    m_videoTrack = nt;
    Segment::assignTrack(track);

    if ( m_filtersObject )
        addWatcher();
}

void ImageSegment::cursorEnter(qint64 pos){
    if ( !m_videoTrack || !m_videoTrack->surface() || !m_image)
        return;

    frameCaptured(m_image->cloneMat(), position() + pos);
}

void ImageSegment::cursorExit(qint64){
    if ( !m_videoTrack || !m_videoTrack->surface() || !m_image)
        return;
}

void ImageSegment::cursorNext(qint64 pos){
    if ( !m_videoTrack || !m_videoTrack->surface() || !m_image)
        return;

    frameCaptured(m_image->cloneMat(), position() + pos);
}

void ImageSegment::cursorMove(qint64 pos){
    if ( !m_videoTrack || !m_videoTrack->surface() || !m_image)
        return;

    frameCaptured(m_image->cloneMat(), position() + pos);
}

void ImageSegment::filtersStreamHandler(QObject *that, const QJSValue &val){
    ImageSegment* vs = static_cast<ImageSegment*>(that);
    vs->streamUpdate(val);
}

void ImageSegment::streamUpdate(const QJSValue &val){
    QMat* frame = static_cast<QMat*>(val.toQObject());

    if ( m_filtersPosition > -1 ){
        m_videoTrack->surface()->updateSurface(m_filtersPosition, frame);
        qint64 processedPosition = m_filtersPosition;
        m_filtersPosition = -1;
        if ( isProcessing() ){
            setIsProcessing(false);
            m_videoTrack->notifyCursorProcessed(processedPosition);
        }
    } else {
        m_videoTrack->refreshPosition();
    }
}

void ImageSegment::frameCaptured(QMat *frame, qint64 position){
    if ( m_filtersObject ){
        setIsProcessing(true);
        m_filtersPosition = position;
        m_filtersObject->pull()->push(frame);
    } else {
        m_videoTrack->surface()->updateSurface(position, frame);
    }
}

void ImageSegment::createFilters(){
    if ( m_filtersObject ){
        m_filtersObject->deleteLater();
        m_filtersObject = nullptr;
    }

    if ( !m_filters.isEmpty() ){
        ViewEngine* ve = ViewContext::instance().engine();
        ViewEngine::ComponentResult::Ptr cr = ve->createObject(m_filters, this);
        if ( cr->hasError() ){
            QmlError::join(cr->errors).jsThrow();
            return;
        }

        QmlStreamFilter* sfilter = qobject_cast<QmlStreamFilter*>(cr->object);
        if ( !sfilter ){
            //TODO:
            qWarning("Not of filter object.");
        }

        m_filtersObject = sfilter;
        m_filtersObject->setPull(new QmlStream(m_filtersObject));
        m_filtersObject->result()->stream()->forward(this, &ImageSegment::filtersStreamHandler);

        emit filtersObjectChanged();

        if ( m_videoTrack ){
            addWatcher();
        }
    }
}

void ImageSegment::addWatcher(){
    HookContainer* hooks = qobject_cast<HookContainer*>(m_videoTrack->timelineContext()->contextProperty("hooks").value<QObject*>());
    if ( !hooks ){
        Exception e = CREATE_EXCEPTION(lv::Exception, "ImageSegment cannot access the hook container.", Exception::toCode("~hooks"));
        lv::ViewContext::instance().engine()->throwError(&e, this);
        return;
    }
    ViewEngine* ve = ViewContext::instance().engine();

    QmlWatcher* watcher = new QmlWatcher(m_filtersObject);

    //TODO: Generate unique name
    watcher->setTarget(m_filtersObject);
    watcher->initialize(ve, hooks, m_filters, m_videoTrack->name() + "_" + QString::number(position()));
}

}// namespace
