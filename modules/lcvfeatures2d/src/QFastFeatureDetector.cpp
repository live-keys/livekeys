#include "QFastFeatureDetector.hpp"
#include "opencv2/features2d/features2d.hpp"

QFastFeatureDetector::QFastFeatureDetector(QQuickItem *parent)
    : QFeatureDetector(new cv::FastFeatureDetector, parent)
{
}

QFastFeatureDetector::~QFastFeatureDetector(){
}
