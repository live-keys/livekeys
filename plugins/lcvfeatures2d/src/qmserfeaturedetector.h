#ifndef QMSERFEATUREDETECTOR_H
#define QMSERFEATUREDETECTOR_H

#include "qfeaturedetector.h"

class QMSerFeatureDetector : public QFeatureDetector{

    Q_OBJECT

public:
    QMSerFeatureDetector(QQuickItem* parent = 0);
    ~QMSerFeatureDetector();

public slots:
    void initialize(
        int delta = 5,
        int minArea = 60,
        int maxArea = 14400,
        float maxVariation = 0.25,
        float minDiversity = 0.2,
        int maxEvolution = 200,
        double areaThreshold = 1.01,
        double minMargin = 0.003,
        int edgeBlurSize = 5
    );
};


#endif // QMSERFEATUREDETECTOR_H
