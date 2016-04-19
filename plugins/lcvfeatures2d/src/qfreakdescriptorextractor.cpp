#include "qfreakdescriptorextractor.h"
#include "opencv2/features2d/features2d.hpp"

QFreakDescriptorExtractor::QFreakDescriptorExtractor(QQuickItem *parent) :
    QDescriptorExtractor(new cv::FREAK, parent)
{
}

QFreakDescriptorExtractor::~QFreakDescriptorExtractor(){
}

void QFreakDescriptorExtractor::initialize(const QVariantMap &params){
    bool orientationNormalized = true;
    bool scaleNormalized       = true;
    float patternScale         = 22.0f;
    int nOctaves               = 4;

    if ( params.contains("orientationNormalized") )
        orientationNormalized = params["orientationNormalized"].toBool();
    if ( params.contains("scaleNormalized") )
        scaleNormalized = params["scaleNormalized"].toBool();
    if ( params.contains("patternScale") )
        patternScale = params["patternScale"].toFloat();
    if ( params.contains("nOctaves") )
        nOctaves = params["nOctaves"].toInt();

    initializeExtractor(new cv::FREAK(orientationNormalized, scaleNormalized, patternScale, nOctaves));
}
