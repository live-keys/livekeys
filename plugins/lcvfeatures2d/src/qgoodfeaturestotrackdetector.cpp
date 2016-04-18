#include "qgoodfeaturestotrackdetector.h"
#include "opencv2/features2d/features2d.hpp"

QGoodFeaturesToTrackDetector::QGoodFeaturesToTrackDetector(QQuickItem *parent)
    : QFeatureDetector(new cv::GoodFeaturesToTrackDetector, parent)

{
}

QGoodFeaturesToTrackDetector::~QGoodFeaturesToTrackDetector(){
}

void QGoodFeaturesToTrackDetector::initialize(const QVariantMap &settings){
    int maxCorners         = 1000;
    double qualityLevel    = 0.01;
    double minDistance     = 1;
    int blockSize          = 3;
    bool useHarrisDetector = false;
    double k               = 0.04;

    if ( settings.contains("maxCorners") )
        maxCorners = settings["maxCorners"].toInt();
    if ( settings.contains("qualityLevel") )
        qualityLevel = settings["qualityLevel"].toDouble();
    if ( settings.contains("minDistance") )
        minDistance = settings["minDistance"].toDouble();
    if ( settings.contains("blockSize") )
        blockSize = settings["blockSize"].toInt();
    if ( settings.contains("useHarrisDetector") )
        useHarrisDetector = settings["useHarrisDetector"].toBool();
    if ( settings.contains("k") )
        k = settings["k"].toDouble();

    initializeDetector(new cv::GoodFeaturesToTrackDetector(
        maxCorners, qualityLevel, minDistance, blockSize, useHarrisDetector, k
    ));
}
