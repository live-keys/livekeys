#include "qtonemapdurand.h"

QTonemapDurand::QTonemapDurand(QQuickItem *parent)
    : QTonemap(cv::createTonemapDurand(), parent)
{
}

void QTonemapDurand::initialize(const QVariantMap &options){
    float gamma = 1.0f;
    float contrast = 4.0f;
    float saturation = 1.0f;
    float sigmaSpace = 2.0f;
    float sigmaColor = 2.0f;

    if ( options.contains("gamma") )
        gamma = options["gamma"].toFloat();
    if ( options.contains("contrast") )
        contrast = options["contrast"].toFloat();
    if ( options.contains("saturation") )
        saturation = options["saturation"].toFloat();
    if ( options.contains("sigmaSpace") )
        sigmaSpace = options["sigmaSpace"].toFloat();
    if ( options.contains("sigmaColor") )
        sigmaColor = options["sigmaColor"].toFloat();

    initializeTonemapper(cv::createTonemapDurand(
        gamma, contrast, saturation, sigmaSpace, sigmaColor
    ));

}

