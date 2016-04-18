#ifndef QGOODFEATURESTOTRACKDETECTOR_HPP
#define QGOODFEATURESTOTRACKDETECTOR_HPP

#include "qfeaturedetector.h"

class QGoodFeaturesToTrackDetector : public QFeatureDetector{

    Q_OBJECT

public:
    explicit QGoodFeaturesToTrackDetector(QQuickItem *parent = 0);
    ~QGoodFeaturesToTrackDetector();

public slots:
    void initialize(const QVariantMap& settings);

};

#endif // QGOODFEATURESTOTRACKDETECTOR_HPP
