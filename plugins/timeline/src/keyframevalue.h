#ifndef LVKEYFRAMEVALUE_H
#define LVKEYFRAMEVALUE_H

#include <QObject>
#include "timeline.h"

namespace lv{

class KeyframeTrack;
class KeyframeValue : public QObject{

    Q_OBJECT
    Q_PROPERTY(lv::Timeline* timeline READ timeline WRITE setTimeline NOTIFY timelineChanged)
    Q_PROPERTY(QString track          READ track    WRITE setTrack NOTIFY trackChanged)
    Q_PROPERTY(double value           READ value    NOTIFY valueChanged)

public:
    explicit KeyframeValue(QObject *parent = nullptr);
    virtual ~KeyframeValue() override;

    double value() const;
    void setValue(double value);
    const QString &track() const;
    Timeline* timeline() const;

    void setTrack(QString track);
    void setTimeline(lv::Timeline* timeline);

signals:
    void valueChanged();
    void trackChanged();
    void timelineChanged();

public slots:
    void __readValue();
    void __trackAdded(Track* track);
    void __trackAboutToBeRemoved(Track* track);

private:
    void findTrack();

    double         m_value;
    QString        m_track;
    KeyframeTrack* m_trackObject;
    Timeline*      m_timeline;
};

inline double KeyframeValue::value() const{
    return m_value;
}

inline const QString& KeyframeValue::track() const{
    return m_track;
}

inline Timeline *KeyframeValue::timeline() const{
    return m_timeline;
}

inline void KeyframeValue::setTrack(QString track){
    if (m_track == track)
        return;

    m_track = track;
    emit trackChanged();

    findTrack();
}

}// namespace

#endif // LVKEYFRAMEVALUE_H
