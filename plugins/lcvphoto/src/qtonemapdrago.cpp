#include "qtonemapdrago.h"

QTonemapDrago::QTonemapDrago(QQuickItem *parent)
    : QTonemap(cv::createTonemapDrago(), parent)
{
}

void QTonemapDrago::initialize(const QVariantMap &options){
    float gamma = 1.0f;
    float saturation =   1.0f;
    float bias = 0.85f;

    if ( options.contains("gamma") )
        gamma = options["gamma"].toFloat();
    if ( options.contains("saturation") )
        saturation = options["saturation"].toFloat();
    if ( options.contains("bias") )
        bias = options["bias"].toFloat();

    initializeTonemapper(cv::createTonemapDrago(gamma, saturation, bias));
}
