#ifndef QBRISKFEATUREDETECTOR_H
#define QBRISKFEATUREDETECTOR_H

#include "qfeaturedetector.h"

class QBriskFeatureDetector : public QFeatureDetector{

    Q_OBJECT

public:
    explicit QBriskFeatureDetector(QQuickItem *parent = 0);
    virtual ~QBriskFeatureDetector();

    void initialize(const QVariantMap& settings);

};

#endif // QBRISKFEATUREDETECTOR_H
