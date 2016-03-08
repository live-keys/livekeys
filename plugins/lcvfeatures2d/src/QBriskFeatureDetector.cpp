#include "QBriskFeatureDetector.hpp"
#include "opencv2/features2d/features2d.hpp"

QBriskFeatureDetector::QBriskFeatureDetector(QQuickItem *parent) :
    QFeatureDetector(new cv::BRISK, parent)
{
}

QBriskFeatureDetector::~QBriskFeatureDetector(){
}

void QBriskFeatureDetector::initialize(int thresh, int octaves, float patternScale){
    initializeDetector(new cv::BRISK(thresh, octaves, patternScale));
}
