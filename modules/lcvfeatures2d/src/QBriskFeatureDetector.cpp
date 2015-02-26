#include "QBriskFeatureDetector.hpp"
#include "opencv2/features2d/features2d.hpp"

QBriskFeatureDetector::QBriskFeatureDetector(QQuickItem *parent) :
    QFeatureDetector(new cv::BRISK, parent)
{
    this->detector()->setInt("thresh", 30);
}

QBriskFeatureDetector::~QBriskFeatureDetector(){
}
