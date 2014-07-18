#include "QMatResize.hpp"
#include "opencv2/imgproc/imgproc.hpp"

using namespace cv;

QMatResize::QMatResize(QQuickItem *parent)
    : QMatFilter(parent)
    , m_matSize(0, 0)
    , m_fx(0)
    , m_fy(0)
    , m_interpolation(INTER_LINEAR)
{
}

QMatResize::~QMatResize(){
}

void QMatResize::transform(cv::Mat& in, cv::Mat& out){
    if ( !in.empty() && (m_matSize != Size(0, 0) || m_fx != 0 || m_fy != 0) )
        resize(in, out, m_matSize, m_fx, m_fy, m_interpolation);
}
