#ifndef QAUTOLEVELS_H
#define QAUTOLEVELS_H

#include <QObject>
#include "qmat.h"

class QAutoLevels : public QObject{

    Q_OBJECT
    Q_PROPERTY(QMat* histogram READ histogram WRITE setHistogram NOTIFY histogramChanged)
    Q_PROPERTY(QJSValue output READ output    NOTIFY outputChanged)

public:
    explicit QAutoLevels(QObject *parent = nullptr);

    QMat* histogram() const;
    QJSValue output() const;

signals:
    void histogramChanged();
    void outputChanged();

public slots:
    void setHistogram(QMat* histogram);

private:
    QMat*    m_histogram;
    QJSValue m_output;
};

inline QMat *QAutoLevels::histogram() const{
    return m_histogram;
}

inline QJSValue QAutoLevels::output() const{
    return m_output;
}

#endif // QAUTOLEVELS_H
