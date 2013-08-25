#include "QMatTransformation.hpp"

QMatTransformation::QMatTransformation(QQuickItem *parent) :
    QMatDisplay(parent),
    m_in(new QMat(this))
{
}

QMatTransformation::~QMatTransformation(){
}


void QMatTransformation::transform(cv::Mat&, cv::Mat&){
}
