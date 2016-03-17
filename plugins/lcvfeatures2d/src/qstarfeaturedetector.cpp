#include "qstarfeaturedetector.h"
#include "opencv2/features2d/features2d.hpp"

QStarFeatureDetector::QStarFeatureDetector(QQuickItem *parent)
    : QFeatureDetector(new cv::StarDetector, parent)
{
}

QStarFeatureDetector::~QStarFeatureDetector(){
}

void QStarFeatureDetector::initialize(
        int maxSize, int responseThreshold, int lineThresholdProjected,
        int lineThresholdBinarized, int suppressNonmaxSize)
{
    initializeDetector(new cv::StarDetector(
        maxSize, responseThreshold, lineThresholdProjected, lineThresholdBinarized, suppressNonmaxSize
    ));
}
