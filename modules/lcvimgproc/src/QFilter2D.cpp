#include "QFilter2D.hpp"
#include "opencv2/imgproc/imgproc.hpp"

using namespace cv;

QFilter2D::QFilter2D(QQuickItem *parent)
    : QMatFilter(parent)
    , m_anchorCv(-1, -1)
    , m_borderType(BORDER_DEFAULT)
{
}

QFilter2D::~QFilter2D(){
}

void QFilter2D::transform(cv::Mat &in, cv::Mat &out){
    if ( m_kernel ){
        filter2D(in, out, m_ddepth, *(m_kernel->cvMat()), m_anchorCv, m_borderType);
    }
}
