#include "qfastfeaturedetector.h"
#include "opencv2/features2d/features2d.hpp"

QFastFeatureDetector::QFastFeatureDetector(QQuickItem *parent)
    : QFeatureDetector(new cv::FastFeatureDetector, parent)
{
}

QFastFeatureDetector::~QFastFeatureDetector(){
}

void QFastFeatureDetector::initialize(const QVariantMap& settings){
    int threshold = 10;
    bool nonmaxSuppression = true;

    if ( settings.contains("threshold") )
        threshold = settings["threshold"].toInt();
    if ( settings.contains("nonmaxSuppresion") )
        nonmaxSuppression = settings["nonmaxSuppresion"].toBool();

    initializeDetector(new cv::FastFeatureDetector(threshold, nonmaxSuppression));
}
