#ifndef QDENSEFEATUREDETECTOR_HPP
#define QDENSEFEATUREDETECTOR_HPP

#include "qfeaturedetector.h"

class QDenseFeatureDetector : public QFeatureDetector{

    Q_OBJECT

public:
    explicit QDenseFeatureDetector(QQuickItem *parent = 0);
    virtual ~QDenseFeatureDetector();

public slots:
    void initialize(
            float initFeatureScale = 1.f, int featureScaleLevels = 1, float featureScaleMul = 0.1f, int initXyStep=6,
            int initImgBound = 0,bool varyXyStepWithScale = true, bool varyImgBoundWithScale = false
    );

};

#endif // QDENSEFEATUREDETECTOR_HPP
