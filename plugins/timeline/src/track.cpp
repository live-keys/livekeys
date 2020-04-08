#include "track.h"
#include "segmentmodel.h"
#include "segment.h"

#include "live/visuallogqt.h"
#include "live/viewcontext.h"
#include "live/viewengine.h"
#include "live/exception.h"

namespace lv{

Track::Track(QObject *parent)
    : QObject(parent)
    , m_segmentModel(new SegmentModel(this))
    , m_cursorPosition(0)
    , m_activeSegment(nullptr)
{
    connect(m_segmentModel, &QAbstractRangeModel::itemsChanged, this, &Track::__segmentModelItemsChanged);
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
            m_activeSegment->cursorExit();
            m_activeSegment = nullptr;
        }
    }

    if ( !m_activeSegment ){
        Segment* segm = m_segmentModel->segmentThatWraps(newPosition);
        if ( segm ){
            segm->cursorEnter(newPosition - segm->position());
        }
        m_activeSegment = segm;
    }
    m_cursorPosition = newPosition;

    return CursorOperation::Ready;
}

void Track::addSegment(Segment *segment){
    m_segmentModel->insertItem(segment);
}

//TODO: Condition: If m_activeSegment gets removed (connect to the segmentModel)
void Track::__segmentModelItemsChanged(qint64, qint64){

}

}// namespace
