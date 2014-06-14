#include "QGaussianBlur.hpp"
#include "opencv2/imgproc/imgproc.hpp"

using namespace cv;

QGaussianBlur::QGaussianBlur(QQuickItem *parent)
    : QMatFilter(parent)
    , m_ksize(3)
    , m_borderType(BORDER_DEFAULT)
{
}

QGaussianBlur::~QGaussianBlur(){
}

void QGaussianBlur::transform(cv::Mat &in, cv::Mat &out){
    GaussianBlur(in, out, cv::Size(m_ksize.width(), m_ksize.height()), m_sigmaX, m_sigmaY, m_borderType);
}
