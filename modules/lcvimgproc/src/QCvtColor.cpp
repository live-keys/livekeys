#include "QCvtColor.hpp"
#include "opencv2/imgproc/imgproc.hpp"

using namespace cv;

QCvtColor::QCvtColor(QQuickItem *parent) :
    QMatFilter(parent)
{
}

void QCvtColor::transform(cv::Mat &in, cv::Mat &out){
    cvtColor(in, out, m_code, m_dstCn);
}
