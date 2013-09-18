#include "QMatFilter.hpp"

QMatFilter::QMatFilter(QQuickItem *parent) :
    QMatDisplay(parent),
    m_in(new QMat(this))
{
}

QMatFilter::~QMatFilter(){
}


void QMatFilter::transform(cv::Mat&, cv::Mat&){
}
