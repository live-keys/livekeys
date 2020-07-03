#ifndef LVNUMBERANIMATIONSEGMENT
#define LVNUMBERANIMATIONSEGMENT

#include <QObject>
#include <QEasingCurve>

#include "live/segment.h"
#include "numbertrack.h"

namespace lv{

class NumberAnimationSegment : public Segment{

    Q_OBJECT
    Q_PROPERTY(QEasingCurve easing READ easing WRITE setEasing NOTIFY easingChanged)
    Q_PROPERTY(double from         READ from   WRITE setFrom   NOTIFY fromChanged)
    Q_PROPERTY(double to           READ to     WRITE setTo     NOTIFY toChanged)

public:
    explicit NumberAnimationSegment(QObject *parent = nullptr);
    ~NumberAnimationSegment();

    double value() const;

    const QEasingCurve& easing() const;
    void setEasing(QEasingCurve easing);

    void serialize(QQmlEngine *engine, MLNode &node) const override;
    void deserialize(Track *track, QQmlEngine *engine, const MLNode &data) override;

    void assignTrack(Track *track) override;
    void cursorPass(qint64 position) override;
    void cursorEnter(qint64 position) override;
    void cursorExit(qint64 position) override;
    void cursorNext(qint64 position) override;
    void cursorMove(qint64 position) override;

    void setFrom(double from);
    double from() const;

    void setTo(double to);
    double to() const;

public slots:
    void resetCurve();

signals:
    void valueChanged();
    void easingChanged();
    void fromChanged();
    void toChanged();

private:
    void setValue(double value);

private:
    NumberTrack*  m_track;
    QEasingCurve  m_easing;
    double        m_from;
    double        m_to;
    QList<double> m_curve;
};

inline const QEasingCurve& NumberAnimationSegment::easing() const{
    return m_easing;
}

inline void NumberAnimationSegment::setEasing(QEasingCurve easing){
    if (m_easing == easing)
        return;

    m_easing = easing;
    emit easingChanged();

    resetCurve();
}

inline double NumberAnimationSegment::from() const{
    return m_from;
}

inline double NumberAnimationSegment::to() const{
    return m_to;
}

inline void NumberAnimationSegment::setFrom(double from){
    if (qFuzzyCompare(m_from, from))
        return;

    m_from = from;
    emit fromChanged();

    setValue(m_from);

    resetCurve();
}

inline void NumberAnimationSegment::setTo(double to){
    if (qFuzzyCompare(m_to, to))
        return;

    m_to = to;
    emit toChanged();

    resetCurve();
}

}// namespace

#endif // LVNUMBERANIMATIONSEGMENT
