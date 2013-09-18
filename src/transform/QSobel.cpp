#include "QSobel.hpp"
#include "opencv2/imgproc/imgproc.hpp"

using namespace cv;

QSobel::QSobel(QQuickItem *parent) :
    QMatFilter(parent){
}

QSobel::~QSobel(){
}

void QSobel::transform(cv::Mat &in, cv::Mat &out){
    Sobel(in, out, m_ddepth, m_xorder, m_yorder, m_ksize, m_scale, m_delta, m_borderType);
}
