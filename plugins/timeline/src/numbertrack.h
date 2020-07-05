#ifndef LVNUMBERTRACK_H
#define LVNUMBERTRACK_H

#include <QObject>
#include "live/track.h"

namespace lv{

class NumberTrack : public Track{

    Q_OBJECT
    Q_PROPERTY(double value READ value WRITE setValue NOTIFY valueChanged)

public:
    explicit NumberTrack(QObject *parent = nullptr);
    virtual ~NumberTrack();

    double value() const;
    void setValue(double value);

signals:
    void valueChanged(double value);

private:
    double m_value;
};

inline double NumberTrack::value() const{
    return m_value;
}

inline void NumberTrack::setValue(double value){
    if (qFuzzyCompare(m_value, value))
        return;

    m_value = value;
    emit valueChanged(m_value);
}

}// namespace

#endif // LVNUMBERTRACK_H
