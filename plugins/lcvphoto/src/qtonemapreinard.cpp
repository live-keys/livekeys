#include "qtonemapreinard.h"

QTonemapReinard::QTonemapReinard(QQuickItem *parent)
    : QTonemap(cv::createTonemapDurand(), parent)
{
}

void QTonemapReinard::initialize(const QVariantMap &options){
    float gamma = 1.0f;
    float intensity = 0.0f;
    float lightAdapt = 1.0f;
    float colorAdapt = 0.0f;

    if ( options.contains("gamma") )
        gamma = options["gamma"].toFloat();
    if ( options.contains("intensity") )
        intensity = options["intensity"].toFloat();
    if ( options.contains("lightAdapt") )
        lightAdapt = options["lightAdapt"].toFloat();
    if ( options.contains("colorAdapt") )
        colorAdapt = options["colorAdapt"].toFloat();

    initializeTonemapper(cv::createTonemapReinhard(
        gamma, intensity, lightAdapt, colorAdapt
    ));

}
