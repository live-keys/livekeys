#include "QBriefDescriptorExtractor.hpp"
#include "opencv2/features2d/features2d.hpp"

QBriefDescriptorExtractor::QBriefDescriptorExtractor(QQuickItem *parent)
    : QDescriptorExtractor(new cv::BriefDescriptorExtractor, parent)
{
}

QBriefDescriptorExtractor::~QBriefDescriptorExtractor(){
}
