#include "QDescriptorMatchFilter.hpp"

QDescriptorMatchFilter::QDescriptorMatchFilter(QQuickItem* parent)
    : QQuickItem(parent)
    , m_matches1to2Out(new QDMatchVector)
{
}

QDescriptorMatchFilter::~QDescriptorMatchFilter(){
}

void QDescriptorMatchFilter::filter(
        const std::vector<std::vector<cv::DMatch> > &src,
        std::vector<std::vector<cv::DMatch> > &dst
){
    //TODO
}

