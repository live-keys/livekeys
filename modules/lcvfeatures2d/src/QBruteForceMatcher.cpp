#include "QBruteForceMatcher.hpp"
#include "opencv2/features2d/features2d.hpp"

QBruteForceMatcher::QBruteForceMatcher(QQuickItem* parent)
    : QDescriptorMatcher(new cv::BFMatcher, parent)
{
}

QBruteForceMatcher::~QBruteForceMatcher(){
}
