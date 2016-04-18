#include "qdensefeaturedetector.h"
#include "opencv2/features2d/features2d.hpp"

QDenseFeatureDetector::QDenseFeatureDetector(QQuickItem *parent) :
    QFeatureDetector(new cv::DenseFeatureDetector, parent)
{
}


QDenseFeatureDetector::~QDenseFeatureDetector(){
}

void QDenseFeatureDetector::initialize(const QVariantMap &settings){
    float initFeatureScale     = 1.f;
    int featureScaleLevels     = 1;
    float featureScaleMul      = 0.1f;
    int initXyStep             = 6;
    int initImgBound           = 0;
    bool varyXyStepWithScale   = true;
    bool varyImgBoundWithScale = false;

    if ( settings.contains("initFeatureScale") )
        initFeatureScale = settings["initFeatureScale"].toFloat();
    if ( settings.contains("featureScaleLevels") )
        featureScaleLevels = settings["featureScaleLevels"].toInt();
    if ( settings.contains("featureScaleMul") )
        featureScaleMul = settings["featureScaleMul"].toFloat();
    if ( settings.contains("initXyStep") )
        initXyStep = settings["initXyStep"].toInt();
    if ( settings.contains("initImgBound") )
        initImgBound = settings["initImgBound"].toInt();
    if ( settings.contains("varyXyStepWithScale") )
        varyXyStepWithScale = settings["varyXyStepWithScale"].toBool();
    if ( settings.contains("varyImgBoundWithScale") )
        varyImgBoundWithScale = settings["varyImgBoundWithScale"].toBool();

    initializeDetector(new cv::DenseFeatureDetector(
        initFeatureScale, featureScaleLevels, featureScaleMul, initXyStep, initImgBound,
        varyXyStepWithScale, varyImgBoundWithScale
    ));
}
