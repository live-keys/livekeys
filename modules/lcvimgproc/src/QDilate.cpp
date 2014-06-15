#include "QDilate.hpp"
#include "opencv2/imgproc/imgproc.hpp"

using namespace cv;

QDilate::QDilate(QQuickItem *parent)
    : QMatFilter(parent)
    , m_kernel(0)
    , m_anchorCv(-1, -1)
    , m_iterations(1)
    , m_borderType(BORDER_CONSTANT)
{
}

QDilate::~QDilate(){
}

void QDilate::transform(cv::Mat &in, cv::Mat &out){
    if ( m_kernel ){
        Scalar borderValue = morphologyDefaultBorderValue();
        if ( m_borderValue.isValid() ){
            if ( in.channels() == 3 )
                borderValue = Scalar(m_borderValue.blue(), m_borderValue.green(), m_borderValue.red());
            else
                borderValue = Scalar(m_borderValue.red());
        }
        dilate(in, out, *(m_kernel->data()), m_anchorCv, m_iterations, m_borderType, borderValue);
    }
}
