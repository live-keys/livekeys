#include "QMatRoi.hpp"

QMatRoi::QMatRoi(QQuickItem *parent) :
    QMatTransformation(parent)
{
}

void QMatRoi::transform(cv::Mat &in, cv::Mat &out){
    if ( in.cols > m_regionX + m_regionWidth && in.rows > m_regionY + m_regionHeight )
        in(cv::Rect(m_regionX, m_regionY, m_regionWidth, m_regionHeight)).copyTo(out);
}
