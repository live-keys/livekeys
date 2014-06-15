#include "QBlur.hpp"
#include "opencv2/imgproc/imgproc.hpp"

using namespace cv;

QBlur::QBlur(QQuickItem *parent)
    : QMatFilter(parent)
    , m_anchor(QPoint(-1, -1))
    , m_borderType(BORDER_DEFAULT)
{
}

QBlur::~QBlur(){
}

void QBlur::transform(cv::Mat &in, cv::Mat &out){
    blur(in, out, Size(m_ksize.width(), m_ksize.height()), Point(m_anchor.x(), m_anchor.y()), m_borderType);
}
