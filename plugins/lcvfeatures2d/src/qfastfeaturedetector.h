#ifndef QFASTFEATUREDETECTOR_HPP
#define QFASTFEATUREDETECTOR_HPP

#include "qfeaturedetector.h"

class QFastFeatureDetector : public QFeatureDetector{

    Q_OBJECT

public:
    explicit QFastFeatureDetector(QQuickItem *parent = 0);
    ~QFastFeatureDetector();

public slots:
    void initialize(int threshold = 10, bool nonmaxSuppression = true);

};

#endif // QFASTFEATUREDETECTOR_HPP
