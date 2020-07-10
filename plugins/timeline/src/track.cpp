#include "track.h"
#include "timeline.h"
#include "segmentmodel.h"
#include "segment.h"

#include "live/applicationcontext.h"
#include "live/visuallogqt.h"
#include "live/viewcontext.h"
#include "live/viewengine.h"
#include "live/exception.h"
#include "live/qmlerror.h"

#include "live/mlnodetoqml.h"

#include <QJSValue>
#include <QJSValueIterator>
#include <QQmlEngine>
#include <QQmlComponent>

#include <QFile>

#include <QDebug>

namespace lv{

Track::Track(QObject *parent)
    : QObject(parent)
    , m_segmentModel(new SegmentModel(this))
    , m_cursorPosition(0)
    , m_activeSegment(nullptr)
{
    connect(m_segmentModel, &QAbstractRangeModel::itemsChanged, this, &Track::__segmentModelItemsChanged);
}

Track::~Track(){
}

void Track::appendSegmentToList(QQmlListProperty<QObject> *list, QObject *ob){
    Track* that = reinterpret_cast<Track*>(list->data);
    Segment* segment = qobject_cast<Segment*>(ob);
    if (!segment){
        Exception e = CREATE_EXCEPTION(
            Exception, "Track: Trying to append a child that's not a segment.", Exception::toCode("~Segment")
        );
        ViewContext::instance().engine()->throwError(&e, that);
        return;
    }
    that->addSegment(segment);
}

int Track::segmentCount(QQmlListProperty<QObject> *list){
    return reinterpret_cast<Track*>(list->data)->m_segmentModel->totalSegments();
}

QObject *Track::segmentAt(QQmlListProperty<QObject> *list, int index){
    return reinterpret_cast<Track*>(list->data)->m_segmentModel->segmentAt(index);
}

void Track::clearSegments(QQmlListProperty<QObject> *list){
    return reinterpret_cast<Track*>(list->data)->m_segmentModel->clearSegments();
}

QQmlListProperty<QObject> Track::segments(){
    return QQmlListProperty<QObject>(
        this,
        this,
        &Track::appendSegmentToList,
        &Track::segmentCount,
        &Track::segmentAt,
        &Track::clearSegments);
}

Track::CursorOperation Track::updateCursorPosition(qint64 newPosition){
    if ( m_activeSegment ){
        if ( m_activeSegment->contains(newPosition) ){
            if ( newPosition == m_cursorPosition + 1 ){
                m_activeSegment->cursorNext(newPosition - m_activeSegment->position());
            } else {
                m_activeSegment->cursorMove(newPosition - m_activeSegment->position());
            }
        } else {
            m_activeSegment->cursorExit(newPosition);
            m_activeSegment = nullptr;
        }
    }

    if ( !m_activeSegment ){
        QPair<int, int> passedIndexes = m_segmentModel->indexesBetween(m_cursorPosition, newPosition);
        if ( passedIndexes.first != -1 ){
            if ( m_cursorPosition > newPosition ){ // reverse
                for ( int i = passedIndexes.second; i >= passedIndexes.first; --i ){
                    m_segmentModel->segmentAt(i)->cursorPass(newPosition);
                }
            } else { // forward
                for ( int i = passedIndexes.first; i <= passedIndexes.second; ++i ){
                    m_segmentModel->segmentAt(i)->cursorPass(newPosition);
                }
            }
        }

        Segment* segm = m_segmentModel->segmentThatWraps(newPosition);
        if ( segm ){
            segm->cursorEnter(newPosition - segm->position());
        }
        m_activeSegment = segm;
    }
    m_cursorPosition = newPosition;

    return CursorOperation::Ready;
}

void Track::serialize(ViewEngine *engine, const QObject *o, MLNode &node){
    const Track* track = qobject_cast<const Track*>(o);
    track->serialize(engine, node);
}

void Track::deserialize(Track *track, ViewEngine *engine, const MLNode &node){
    track->deserialize(engine, node);
}

void Track::serialize(ViewEngine *engine, MLNode &node) const{
    node = MLNode(MLNode::Object);

    node["name"] = m_name.toStdString();

    MLNode segmentsNode = MLNode(MLNode::Array);
    for ( int i = 0; i < m_segmentModel->totalSegments(); ++i ){
        Segment* segm = m_segmentModel->segmentAt(i);
        MLNode segmentNode;
        segm->serialize(engine->engine(), segmentNode);
        segmentsNode.append(segmentNode);
    }

    node["segments"] = segmentsNode;
}

void Track::deserialize(ViewEngine *engine, const MLNode &node){
    segmentModel()->clearSegments();
    setName(QString::fromStdString(node["name"].asString()));

    const MLNode::ArrayType& segments = node["segments"].asArray();
    for ( auto it = segments.begin(); it != segments.end(); ++it ){
        const MLNode& segmNode = *it;
        if ( segmNode["type"].asString() == "Segment" ){
            Segment* segment = new Segment;
            segment->deserialize(this, engine->engine(), segmNode);
            addSegment(segment);
        } else {
            ViewEngine::ComponentResult::Ptr segmentComp = engine->createPluginObject(QString::fromStdString(segmNode["factory"].asString()), nullptr);
            if ( segmentComp->hasError() ){
                segmentComp->jsThrowError();
            } else {
                Segment* segment = nullptr;

                QVariant result;
                QMetaObject::invokeMethod(segmentComp->object, "create", Qt::DirectConnection, Q_RETURN_ARG(QVariant, result));

                segment = qobject_cast<Segment*>(result.value<QObject*>());
                if ( segment ){
                    segment->deserialize(this, engine->engine(), segmNode);
                    addSegment(segment);
                }
            }
        }
    }
}

QJSValue Track::timelineProperties() const{
    Timeline* timeline = qobject_cast<Timeline*>(parent());
    if ( timeline )
        return timeline->properties();
    return QJSValue();
}

bool Track::addSegment(Segment *segment){
    if ( m_segmentModel->addSegment(segment) ){
        segment->assignTrack(this);
        return true;
    }
    return false;
}

Segment *Track::takeSegment(Segment *segment){
    Segment* result = m_segmentModel->takeSegment(segment);
    if ( result ){
        result->assignTrack(nullptr);
    }
    return result;
}

qint64 Track::availableSpace(qint64 position){
    return m_segmentModel->availableSpace(position);
}

Timeline* Track::timeline(){
    return qobject_cast<Timeline*>(parent());
}

//TODO: Condition: If m_activeSegment gets removed (connect to the segmentModel)
void Track::__segmentModelItemsChanged(qint64, qint64){

}

}// namespace
