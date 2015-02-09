#ifndef QFASTFEATUREDETECTOR_HPP
#define QFASTFEATUREDETECTOR_HPP

#include "QFeatureDetector.hpp"

class QFastFeatureDetector : public QFeatureDetector{

    Q_OBJECT

public:
    explicit QFastFeatureDetector(QQuickItem *parent = 0);
    ~QFastFeatureDetector();

signals:

public slots:

};

#endif // QFASTFEATUREDETECTOR_HPP
