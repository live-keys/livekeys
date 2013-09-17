#include "QMat2DArray.hpp"

QMat2DArray::QMat2DArray(QQuickItem *parent)
    : QMatDisplay(parent)
    , m_type(QMat::CV8U)
{
}

QMat2DArray::~QMat2DArray(){
}

cv::Size QMat2DArray::assignValues(){
    val.size();
    return cv::Size(0, 0);
}
