#include "qdensefeaturedetector.h"
#include "opencv2/features2d/features2d.hpp"

QDenseFeatureDetector::QDenseFeatureDetector(QQuickItem *parent) :
    QFeatureDetector(new cv::DenseFeatureDetector, parent)
{
}


QDenseFeatureDetector::~QDenseFeatureDetector(){
}

void QDenseFeatureDetector::initialize(
        float initFeatureScale, int featureScaleLevels, float featureScaleMul, int initXyStep,
        int initImgBound, bool varyXyStepWithScale, bool varyImgBoundWithScale)
{
    initializeDetector(new cv::DenseFeatureDetector(
        initFeatureScale, featureScaleLevels, featureScaleMul, initXyStep, initImgBound,
        varyXyStepWithScale, varyImgBoundWithScale
    ));
}
