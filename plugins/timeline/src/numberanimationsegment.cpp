#include "numberanimationsegment.h"
#include "live/visuallogqt.h"

#include "live/viewcontext.h"
#include "live/viewengine.h"
#include "live/exception.h"

namespace lv{


NumberAnimationSegment::NumberAnimationSegment(QObject *parent)
    : Segment(parent)
    , m_track(nullptr)
    , m_from(0)
    , m_to(0)
{
}

NumberAnimationSegment::~NumberAnimationSegment(){
}

void NumberAnimationSegment::serialize(QQmlEngine *engine, MLNode &node) const{
    Segment::serialize(engine, node);
    node["from"] = m_from;
    node["to"] = m_to;
    node["type"] = "NumberAnimationSegment";
    node["factory"] = "timeline/NumberAnimationSegmentFactory.qml";
}

void NumberAnimationSegment::deserialize(Track *track, QQmlEngine *engine, const MLNode &node){
    Segment::deserialize(track, engine, node);

    setFrom(node["from"].asInt());
    setTo(node["to"].asInt());
}

void NumberAnimationSegment::assignTrack(Track *track){
    NumberTrack* nt = qobject_cast<NumberTrack*>(track);
    if ( !nt ){
        Exception e = CREATE_EXCEPTION(lv::Exception, "NumberAnimationSegment needs NumberTrack at '" + track->name().toStdString() + "'.", Exception::toCode("~Track") );
        lv::ViewContext::instance().engine()->throwError(&e, this);
        return;
    }
    m_track = nt;
}

void NumberAnimationSegment::cursorPass(qint64 position){
    cursorExit(position);
}

void NumberAnimationSegment::cursorEnter(qint64 position){
    if ( position >= m_curve.length() )
        return;

    setValue(m_curve[position]);
}

void NumberAnimationSegment::cursorExit(qint64 position){
    if ( position < Segment::position() ){
        setValue(m_from);
    } else if ( position >= Segment::position() + length() )
        setValue(m_to);
}

void NumberAnimationSegment::cursorNext(qint64 position){
    if ( position >= m_curve.length() )
        return;

    setValue(m_curve[position]);
}

void NumberAnimationSegment::cursorMove(qint64 position){
    if ( position >= m_curve.length() )
        return;

    setValue(m_curve[position]);
}

void NumberAnimationSegment::resetCurve(){
    m_curve = QList<double>();
    if ( length() == 0 )
        return;

    double step = 1.0 / length();
    double multiplier = m_to - m_from;

    for (double t = 0.0; t < 1.0; t += step ){
        double val = m_easing.valueForProgress(t);
        double regularized = m_from + (val * multiplier);

        m_curve.append(regularized > 0 ? regularized : 0);
    }

    double val = m_easing.valueForProgress(1.0);
    double regularized = m_from + (val * multiplier);
    m_curve[m_curve.length() - 1] = regularized;
}

void NumberAnimationSegment::setValue(double value){
    if ( !m_track )
        return;

    m_track->setValue(value);
}

double NumberAnimationSegment::value() const{
    if ( !m_track )
        return 0;
    return m_track->value();
}

}// namespace
