#include "keyframetrack.h"

#include "timeline.h"
#include "segmentmodel.h"
#include "keyframe.h"

#include "live/qmlerror.h"
#include "live/exception.h"
#include "live/viewengine.h"

#include "live/visuallogqt.h"

namespace lv{

KeyframeTrack::KeyframeTrack(QObject *parent)
    : Track(parent)
{
    connect(segmentModel(), &QAbstractRangeModel::itemsChanged, this, &KeyframeTrack::__updateKeyframes);
}

KeyframeTrack::~KeyframeTrack(){
}

void KeyframeTrack::updateValue(){
    int np = static_cast<int>(cursorPosition());

    if ( m_values.size() > np )
        setValue(m_values[np]);
}

void KeyframeTrack::setContentLength(qint64 contentLength){
    m_values.resize(static_cast<int>(contentLength));
    Track::setContentLength(contentLength);
}

Track::CursorOperation KeyframeTrack::updateCursorPosition(qint64 newPosition){
    if ( m_values.size() > newPosition ){
        assignCursorPosition(newPosition);
        int np = static_cast<int>(newPosition);
        setValue(m_values[np]);
    }
    return CursorOperation::Ready;
}

void KeyframeTrack::setSegmentPosition(Segment *segment, unsigned int position){
    Keyframe* kf = qobject_cast<Keyframe*>(segment);

    Keyframe* lastPositionNextKf = nullptr;
    int prevIndex = segmentModel()->segmentIndex(kf);
    if ( prevIndex + 1 < segmentModel()->totalSegments() ){
        lastPositionNextKf = qobject_cast<Keyframe*>(segmentModel()->segmentAt(prevIndex + 1));
        lastPositionNextKf->resetCurve();
    }

    Track::setSegmentPosition(segment, position);

    kf->resetCurve();

    Keyframe* nextKf = nullptr;
    int index = segmentModel()->segmentIndex(kf);
    if ( index + 1 < segmentModel()->totalSegments() ){
        nextKf = qobject_cast<Keyframe*>(segmentModel()->segmentAt(index + 1));
        nextKf->resetCurve();
    }

    if ( lastPositionNextKf && lastPositionNextKf != nextKf ){
        lastPositionNextKf->resetCurve();
    }
}

void KeyframeTrack::resetValues(){
    for ( auto it = m_values.begin(); it != m_values.end(); ++it ){
        *it = 0;
    }
}

void KeyframeTrack::serialize(ViewEngine *engine, MLNode &node) const{
    Track::serialize(engine, node);
    node["type"] = "timeline#KeyframeTrack";
    node["factory"] = "timeline/KeyframeTrackFactory.qml";
}

void KeyframeTrack::deserialize(ViewEngine *engine, const MLNode &node){

    Track::deserialize(engine, node);

    const MLNode::ArrayType& segments = node["segments"].asArray();
    for ( auto it = segments.begin(); it != segments.end(); ++it ){
        const MLNode& segmNode = *it;
        if ( segmNode["type"].asString() == "Keyframe" ){
            Keyframe* segment = new Keyframe;
            segment->deserialize(this, engine->engine(), segmNode);
            addSegment(segment);
        } else {
            THROW_QMLERROR(Exception, "KeyframeTrack: Unknown segment type: " + segmNode["type"].asString(), Exception::toCode("~Segment"), this);
            return;
        }
    }
}

void KeyframeTrack::__updateKeyframes(qint64 from, qint64 to){
    int startIndex = segmentModel()->firstIndex(from);
    int endIndex = segmentModel()->firstIndex(to) + 1;
    if ( endIndex >= segmentModel()->totalSegments() ){
        endIndex = segmentModel()->totalSegments() - 1;
    }
    if ( endIndex < 0 ){
        resetValues();
        return;
    }

    for ( int i = startIndex; i <= endIndex; ++i ){
        Keyframe* kf = qobject_cast<Keyframe*>(segmentModel()->segmentAt(i));
        kf->resetCurve();
    }
}

}// namespace
