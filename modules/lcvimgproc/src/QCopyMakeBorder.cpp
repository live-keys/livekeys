#include "QCopyMakeBorder.hpp"
#include "opencv2/imgproc/imgproc.hpp"

using namespace cv;

QCopyMakeBorder::QCopyMakeBorder(QQuickItem *parent)
    : QMatFilter(parent)
    , m_color()
{
}

QCopyMakeBorder::~QCopyMakeBorder(){
}

void QCopyMakeBorder::transform(cv::Mat &in, cv::Mat &out){
    Scalar value;
    if ( m_color.isValid() ){
        if ( out.channels() == 1 )
            value = m_color.red();
        else if ( out.channels() == 3 )
            value = Scalar(m_color.blue(), m_color.green(), m_color.red());
    }
    copyMakeBorder(in, out, m_top, m_bottom, m_left, m_right, m_borderType, value);
}
