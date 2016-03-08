#include "QFreakDescriptorExtractor.hpp"
#include "opencv2/features2d/features2d.hpp"

QFreakDescriptorExtractor::QFreakDescriptorExtractor(QQuickItem *parent) :
    QDescriptorExtractor(new cv::FREAK, parent)
{
}

QFreakDescriptorExtractor::~QFreakDescriptorExtractor(){
}

void QFreakDescriptorExtractor::initialize(
        bool orientationNormalized, bool scaleNormalized, float patternScale, int nOctaves)
{
    initializeExtractor(new cv::FREAK(orientationNormalized, scaleNormalized, patternScale, nOctaves));
}
