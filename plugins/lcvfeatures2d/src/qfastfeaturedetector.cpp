#include "qfastfeaturedetector.h"
#include "opencv2/features2d/features2d.hpp"

QFastFeatureDetector::QFastFeatureDetector(QQuickItem *parent)
    : QFeatureDetector(new cv::FastFeatureDetector, parent)
{
}

QFastFeatureDetector::~QFastFeatureDetector(){
}

void QFastFeatureDetector::initialize(int threshold, bool nonmaxSuppression){
    initializeDetector(new cv::FastFeatureDetector(threshold, nonmaxSuppression));
}
