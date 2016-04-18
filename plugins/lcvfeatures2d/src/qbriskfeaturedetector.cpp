#include "qbriskfeaturedetector.h"
#include "opencv2/features2d/features2d.hpp"

QBriskFeatureDetector::QBriskFeatureDetector(QQuickItem *parent) :
    QFeatureDetector(new cv::BRISK, parent)
{
}

QBriskFeatureDetector::~QBriskFeatureDetector(){
}

void QBriskFeatureDetector::initialize(const QVariantMap &settings){
    int thresh  = 30;
    int octaves = 3;
    float patternScale = 1.0f;
    if ( settings.contains("thresh") )
        thresh = settings["thresh"].toInt();
    if ( settings.contains("octaves") )
        thresh = settings["octaves"].toInt();
    if ( settings.contains("patternScale") )
        patternScale = settings["patternScale"].toFloat();
    initializeDetector(new cv::BRISK(thresh, octaves, patternScale));
}
