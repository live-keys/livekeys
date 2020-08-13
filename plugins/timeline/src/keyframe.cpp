#include "keyframe.h"
#include "segmentmodel.h"

#include "live/visuallogqt.h"
#include "live/viewcontext.h"
#include "live/viewengine.h"
#include "live/exception.h"

namespace lv{

Keyframe::Keyframe(QObject *parent)
    : Segment(parent)
    , m_value(0)
{
    setLength(1);
}

Keyframe::~Keyframe(){
}

void Keyframe::serialize(QQmlEngine *engine, MLNode &node) const{
    Segment::serialize(engine, node);
    node["value"] = m_value;
    node["type"] = "Keyframe";
}

void Keyframe::deserialize(Track *track, QQmlEngine *engine, const MLNode &node){
    Segment::deserialize(track, engine, node);
    setValue(node["value"].asFloat());
}

void Keyframe::assignTrack(Track *track){
    KeyframeTrack* nt = qobject_cast<KeyframeTrack*>(track);
    if ( !nt ){
        Exception e = CREATE_EXCEPTION(lv::Exception, "NumberAnimationSegment needs NumberTrack at '" + track->name().toStdString() + "'.", Exception::toCode("~Track") );
        lv::ViewContext::instance().engine()->throwError(&e, this);
        return;
    }
    Segment::assignTrack(track);
    resetCurve();
}

void Keyframe::resetCurve(){
    KeyframeTrack* track = qobject_cast<KeyframeTrack*>(currentTrack());

    if ( !track )
        return;


    int currentIndex = currentTrack()->segmentModel()->segmentIndex(this);

    if ( currentIndex > 0 ){ // create curve
        int prevIndex = currentIndex - 1;
        Keyframe* segm = qobject_cast<Keyframe*>(currentTrack()->segmentModel()->segmentAt(prevIndex));

        unsigned int positionStart = segm->position();
        unsigned int positionEnd = position();

        unsigned int length = positionEnd - positionStart;

        double from = segm->value();
        double to = m_value;

        double step = 1.0 / length;
        double multiplier = to - from;

        int index = static_cast<int>(positionStart);

        for (double t = 0.0; t < 1.0; t += step ){
            double val = m_easing.valueForProgress(t);
            double regularized = from + (val * multiplier);
            track->m_values[index++] = regularized > 0 ? regularized : 0;
        }
        track->m_values[static_cast<int>(positionEnd)] = to;

    } else { // set value up to this point

        int pos = static_cast<int>(position());
        for ( int i = 0; i < pos; ++i ){
            track->m_values[i] = m_value;
        }
    }

    if ( currentIndex == track->segmentModel()->totalSegments() - 1 ){ // set value after this point
        int pos = static_cast<int>(position());
        for ( int i = pos; i < track->segmentModel()->contentWidth(); ++i ){
            track->m_values[i] = m_value;
        }
    }

    track->updateValue();
}

void Keyframe::setValue(double from){
    if (qFuzzyCompare(m_value, from))
        return;

    m_value = from;
    emit valueChanged();

    setValue(m_value);
    resetCurve();

    KeyframeTrack* track = qobject_cast<KeyframeTrack*>(currentTrack());
    if ( !track )
        return;

    Keyframe* nextKf = nullptr;
    int index = track->segmentModel()->segmentIndex(this);
    if ( index + 1 < track->segmentModel()->totalSegments() ){
        nextKf = qobject_cast<Keyframe*>(track->segmentModel()->segmentAt(index + 1));
        nextKf->resetCurve();
    }

}

}// namespace
