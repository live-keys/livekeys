#include "qorbfeaturedetector.h"
#include "opencv2/features2d/features2d.hpp"

QOrbFeatureDetector::QOrbFeatureDetector(QQuickItem *parent) :
    QFeatureDetector(new cv::ORB, parent)
{
}

QOrbFeatureDetector::~QOrbFeatureDetector()
{
}

void QOrbFeatureDetector::initialize(
        int nfeatures, float scaleFactor, int nlevels, int edgeThreshold, int firstLevel, int WTA_K, int scoreType)
{
    initializeDetector(new cv::ORB(nfeatures, scaleFactor, nlevels, edgeThreshold, firstLevel, WTA_K, scoreType));
}
