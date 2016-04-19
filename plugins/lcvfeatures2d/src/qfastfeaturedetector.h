#ifndef QFASTFEATUREDETECTOR_H
#define QFASTFEATUREDETECTOR_H

#include "qfeaturedetector.h"

class QFastFeatureDetector : public QFeatureDetector{

    Q_OBJECT

public:
    explicit QFastFeatureDetector(QQuickItem *parent = 0);
    ~QFastFeatureDetector();

    void initialize(const QVariantMap& settings);
};

#endif // QFASTFEATUREDETECTOR_H
