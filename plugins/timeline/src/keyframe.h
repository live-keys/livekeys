#ifndef LVKEYFRAME_H
#define LVKEYFRAME_H

#include <QObject>
#include <QEasingCurve>

#include "live/segment.h"
#include "keyframetrack.h"

namespace lv{

class Keyframe : public Segment{

    Q_OBJECT
    Q_PROPERTY(QEasingCurve easing READ easing WRITE setEasing  NOTIFY easingChanged)
    Q_PROPERTY(double value        READ value  WRITE setValue   NOTIFY valueChanged)

public:
    explicit Keyframe(QObject *parent = nullptr);
    virtual ~Keyframe() override;

    const QEasingCurve& easing() const;
    void setEasing(QEasingCurve easing);

    void serialize(QQmlEngine *engine, MLNode &node) const override;
    void deserialize(Track *track, QQmlEngine *engine, const MLNode &data) override;

    void assignTrack(Track *track) override;

    void setValue(double value);
    double value() const;

public slots:
    void resetCurve();

signals:
    void valueChanged();
    void easingChanged();
    void toChanged();

private:
    QEasingCurve   m_easing;
    double         m_value;
};

inline const QEasingCurve& Keyframe::easing() const{
    return m_easing;
}

inline void Keyframe::setEasing(QEasingCurve easing){
    if (m_easing == easing)
        return;

    m_easing = easing;
    emit easingChanged();

    resetCurve();
}

inline double Keyframe::value() const{
    return m_value;
}

}// namespace

#endif // LVKEYFRAME_H
