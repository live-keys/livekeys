#ifndef QGOODFEATURESTOTRACKDETECTOR_HPP
#define QGOODFEATURESTOTRACKDETECTOR_HPP

#include "qfeaturedetector.h"

class QGoodFeaturesToTrackDetector : public QFeatureDetector{

    Q_OBJECT

public:
    explicit QGoodFeaturesToTrackDetector(QQuickItem *parent = 0);
    ~QGoodFeaturesToTrackDetector();

public slots:
    void initialize(
            int maxCorners = 1000, double qualityLevel = 0.01, double minDistance = 1,
            int blockSize=3, bool useHarrisDetector = false, double k = 0.04);

};

#endif // QGOODFEATURESTOTRACKDETECTOR_HPP
