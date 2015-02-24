#include "QBriskFeatureDetector.hpp"
#include "opencv2/features2d/features2d.hpp"

QBriskFeatureDetector::QBriskFeatureDetector(QQuickItem *parent) :
    QFeatureDetector(new cv::BRISK, parent)
{
}

QBriskFeatureDetector::~QBriskFeatureDetector(){
}
