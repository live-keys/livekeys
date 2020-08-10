#ifndef LVKEYFRAMETRACK_H
#define LVKEYFRAMETRACK_H

#include <QObject>
#include "live/track.h"

namespace lv{

class KeyframeTrack : public Track{

    Q_OBJECT
    Q_PROPERTY(double value READ value WRITE setValue NOTIFY valueChanged)

public:
    friend class Keyframe;

public:
    explicit KeyframeTrack(QObject *parent = nullptr);
    virtual ~KeyframeTrack() override;

    double value() const;
    void setValue(double value);
    void updateValue();

    void setContentLength(qint64 contentLength) override;
    virtual CursorOperation updateCursorPosition(qint64 newPosition) override;

    void setSegmentPosition(Segment *segment, unsigned int position) override;
    void resetValues();

    void serialize(ViewEngine *engine, MLNode &node) const override;
    void deserialize(ViewEngine *engine, const MLNode &node) override;

    virtual QString typeReference() const override;

public slots:
    void __updateKeyframes(qint64 from, qint64 to);

signals:
    void valueChanged(double value);

private:
    QVector<double> m_values;
    double          m_value;
};

inline double KeyframeTrack::value() const{
    return m_value;
}

inline void KeyframeTrack::setValue(double value){
    if (qFuzzyCompare(m_value, value))
        return;

    m_value = value;
    emit valueChanged(m_value);
}

inline QString KeyframeTrack::typeReference() const{
    return "timeline#KeyframeTrack";
}

}// namespace

#endif // LVKEYFRAMETRACK_H
