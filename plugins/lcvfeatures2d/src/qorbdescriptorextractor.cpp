#include "qorbdescriptorextractor.h"
#include "opencv2/features2d/features2d.hpp"

QOrbDescriptorExtractor::QOrbDescriptorExtractor(QQuickItem *parent)
    : QDescriptorExtractor(new cv::ORB, parent)
{
}

QOrbDescriptorExtractor::~QOrbDescriptorExtractor(){
}

void QOrbDescriptorExtractor::initialize(const QVariantMap &params){
    int patchSize = 32;
    if ( params.contains("patchSize") )
        patchSize = params["patchSize"].toInt();

    initializeExtractor(
        new cv::ORB(500, 1.2f, 8, 31, 0, 2, cv::ORB::HARRIS_SCORE, patchSize)
    );

}

