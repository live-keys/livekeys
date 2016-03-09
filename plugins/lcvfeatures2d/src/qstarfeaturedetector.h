#ifndef QSTARFEATUREDETECTOR_HPP
#define QSTARFEATUREDETECTOR_HPP

#include "qfeaturedetector.h"

class QStarFeatureDetector : public QFeatureDetector{

    Q_OBJECT

public:
    explicit QStarFeatureDetector(QQuickItem *parent = 0);
    ~QStarFeatureDetector();

public slots:
    void initialize(
            int maxSize = 16, int responseThreshold = 30, int lineThresholdProjected = 10,
            int lineThresholdBinarized = 8, int suppressNonmaxSize = 5);

};

#endif // QSTARFEATUREDETECTOR_HPP
