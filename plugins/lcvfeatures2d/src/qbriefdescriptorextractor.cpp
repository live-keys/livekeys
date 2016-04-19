#include "qbriefdescriptorextractor.h"
#include "opencv2/features2d/features2d.hpp"

QBriefDescriptorExtractor::QBriefDescriptorExtractor(QQuickItem *parent)
    : QDescriptorExtractor(new cv::BriefDescriptorExtractor, parent)
{
}

QBriefDescriptorExtractor::~QBriefDescriptorExtractor(){
}

void QBriefDescriptorExtractor::initialize(const QVariantMap &params){
    int bytes = 32;
    if ( params.contains("bytes") )
        bytes = params["bytes"].toInt();

    initializeExtractor(new cv::BriefDescriptorExtractor(bytes));
}
