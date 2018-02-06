#include "qtonemapmantiuk.h"

QTonemapMantiuk::QTonemapMantiuk(QQuickItem *parent)
    : QTonemap(cv::createTonemapDurand(), parent)
{
}

void QTonemapMantiuk::initialize(const QVariantMap &options){
    float gamma = 1.0f;
    float scale = 0.7f;
    float saturation = 1.0f;

    if ( options.contains("gamma") )
        gamma = options["gamma"].toFloat();
    if ( options.contains("scale") )
        scale = options["scale"].toFloat();
    if ( options.contains("saturation") )
        saturation = options["saturation"].toFloat();

    initializeTonemapper(cv::createTonemapMantiuk(
        gamma, scale, saturation
    ));

}


