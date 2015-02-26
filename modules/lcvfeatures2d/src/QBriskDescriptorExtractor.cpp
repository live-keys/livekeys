#include "QBriskDescriptorExtractor.hpp"
#include "opencv2/features2d/features2d.hpp"

QBriskDescriptorExtractor::QBriskDescriptorExtractor(QQuickItem *parent) :
    QDescriptorExtractor(new cv::BRISK, parent){
}

QBriskDescriptorExtractor::~QBriskDescriptorExtractor(){
}
