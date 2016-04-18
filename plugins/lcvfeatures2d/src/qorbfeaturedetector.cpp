#include "qorbfeaturedetector.h"
#include "opencv2/features2d/features2d.hpp"

QOrbFeatureDetector::QOrbFeatureDetector(QQuickItem *parent) :
    QFeatureDetector(new cv::ORB, parent)
{
}

QOrbFeatureDetector::~QOrbFeatureDetector()
{
}

void QOrbFeatureDetector::initialize(const QVariantMap &settings){
    int nfeatures     = 500;
    float scaleFactor = 1.2f;
    int nlevels       = 8;
    int edgeThreshold = 31;
    int firstLevel    = 0;
    int WTA_K         = 2;
    int scoreType     = HARRIS_SCORE;

    if ( settings.contains("nfeatures") )
        nfeatures = settings["nfeatures"].toInt();
    if ( settings.contains("scaleFactor") )
        scaleFactor = settings["scaleFactor"].toFloat();
    if ( settings.contains("nlevels") )
        nlevels = settings["nlevels"].toInt();
    if ( settings.contains("edgeThreshold") )
        edgeThreshold = settings["edgeThreshold"].toInt();
    if ( settings.contains("firstLevel") )
        firstLevel = settings["firstLevel"].toInt();
    if ( settings.contains("WTA_K") )
        WTA_K = settings["WTA_K"].toInt();
    if ( settings.contains("scoreType") )
        scoreType = settings["scoreType"].toInt();

    initializeDetector(new cv::ORB(nfeatures, scaleFactor, nlevels, edgeThreshold, firstLevel, WTA_K, scoreType));
}
