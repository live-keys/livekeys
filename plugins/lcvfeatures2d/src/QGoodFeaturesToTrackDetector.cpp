#include "QGoodFeaturesToTrackDetector.hpp"
#include "opencv2/features2d/features2d.hpp"

QGoodFeaturesToTrackDetector::QGoodFeaturesToTrackDetector(QQuickItem *parent)
    : QFeatureDetector(new cv::GoodFeaturesToTrackDetector, parent)

{
}

QGoodFeaturesToTrackDetector::~QGoodFeaturesToTrackDetector(){
}

void QGoodFeaturesToTrackDetector::initialize(
        int maxCorners, double qualityLevel, double minDistance, int blockSize, bool useHarrisDetector, double k)
{
    initializeDetector(new cv::GoodFeaturesToTrackDetector(
        maxCorners, qualityLevel, minDistance, blockSize, useHarrisDetector, k
    ));
}
