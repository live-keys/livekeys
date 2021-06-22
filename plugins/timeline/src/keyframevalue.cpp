#include "keyframevalue.h"
#include "live/visuallogqt.h"
#include "keyframetrack.h"

namespace lv{

KeyframeValue::KeyframeValue(QObject *parent)
    : QObject(parent)
    , m_trackObject(nullptr)
    , m_timeline(nullptr)
{
}

KeyframeValue::~KeyframeValue(){
}

void KeyframeValue::setValue(double value){
    if ( qFuzzyCompare(value, m_value) )
        return;

    m_value = value;
    emit valueChanged();
}

void KeyframeValue::__readValue(){
    setValue(m_trackObject->value());
}

void KeyframeValue::__trackAdded(Track *track){
    if ( m_trackObject )
        return;

    if ( track->name() == m_track )
        m_trackObject = qobject_cast<KeyframeTrack*>(track);

    if ( m_trackObject ){
        setValue(m_trackObject->value());
        connect(m_trackObject, &KeyframeTrack::valueChanged, this, &KeyframeValue::__readValue);
    }
}

void KeyframeValue::__trackAboutToBeRemoved(Track *track){
    if ( !m_trackObject )
        return;

    if ( m_trackObject == track ){
        setValue(0);
        disconnect(m_trackObject, &KeyframeTrack::valueChanged, this, &KeyframeValue::__readValue);
        m_trackObject = nullptr;
    }
}

void KeyframeValue::findTrack(){
    if ( m_trackObject ){
        disconnect(m_trackObject, &KeyframeTrack::valueChanged, this, &KeyframeValue::__readValue);
        m_trackObject = nullptr;
    }
    if ( !m_timeline || m_track.isEmpty() )
        return;

    TrackListModel* trackList = timeline()->trackList();

    for ( int i = 0; i < trackList->totalTracks(); ++i ){
        Track* t = trackList->trackAt(i);
        if ( t->name() == m_track ){
            m_trackObject = qobject_cast<KeyframeTrack*>(t);
            break;
        }
    }

    if ( m_trackObject ){
        setValue(m_trackObject->value());
        connect(m_trackObject, &KeyframeTrack::valueChanged, this, &KeyframeValue::__readValue);
    }
}

void KeyframeValue::setTimeline(Timeline *timeline){
    if (m_timeline == timeline)
        return;

    if ( m_timeline ){
        disconnect(m_timeline,              &Timeline::trackNameChanged,            this, &KeyframeValue::__trackAdded);
        disconnect(m_timeline->trackList(), &TrackListModel::trackAdded,            this, &KeyframeValue::__trackAdded);
        disconnect(m_timeline->trackList(), &TrackListModel::trackAboutToBeRemoved, this, &KeyframeValue::__trackAboutToBeRemoved);
    }

    m_timeline = timeline;
    emit timelineChanged();

    if ( m_timeline ){
        connect(m_timeline,              &Timeline::trackNameChanged,            this, &KeyframeValue::__trackAdded);
        connect(m_timeline->trackList(), &TrackListModel::trackAdded,            this, &KeyframeValue::__trackAdded);
        connect(m_timeline->trackList(), &TrackListModel::trackAboutToBeRemoved, this, &KeyframeValue::__trackAboutToBeRemoved);
    }

    findTrack();
}

}// namespace
