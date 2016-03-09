#include "qmserfeaturedetector.h"
#include "opencv2/features2d/features2d.hpp"

QMSerFeatureDetector::QMSerFeatureDetector(QQuickItem *parent)
    : QFeatureDetector(new cv::MSER, parent)
{
}

QMSerFeatureDetector::~QMSerFeatureDetector(){
}

void QMSerFeatureDetector::initialize(
        int delta,
        int minArea,
        int maxArea,
        float maxVariation,
        float minDiversity,
        int maxEvolution,
        double areaThreshold,
        double minMargin,
        int edgeBlurSize
){
    initializeDetector(new cv::MSER(
       delta,
       minArea,
       maxArea,
       maxVariation,
       minDiversity,
       maxEvolution,
       areaThreshold,
       minMargin,
       edgeBlurSize
    ));
}

