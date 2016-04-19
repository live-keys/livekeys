#ifndef QDENSEFEATUREDETECTOR_H
#define QDENSEFEATUREDETECTOR_H

#include "qfeaturedetector.h"

class QDenseFeatureDetector : public QFeatureDetector{

    Q_OBJECT

public:
    explicit QDenseFeatureDetector(QQuickItem *parent = 0);
    virtual ~QDenseFeatureDetector();

    void initialize(const QVariantMap& settings);

};

#endif // QDENSEFEATUREDETECTOR_H
