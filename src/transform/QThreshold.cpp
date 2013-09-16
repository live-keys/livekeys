#include "QThreshold.hpp"
#include "opencv2/imgproc/imgproc.hpp"

QThreshold::QThreshold(QQuickItem *parent) :
    QMatTransformation(parent)
{
}

QThreshold::~QThreshold()
{
}

void QThreshold::transform(cv::Mat &in, cv::Mat &out){
    cv::threshold(in, out, m_thresh, m_maxVal, m_type);
}
