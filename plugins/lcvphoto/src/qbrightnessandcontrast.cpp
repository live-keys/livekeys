#include "qbrightnessandcontrast.h"

QBrightnessAndContrast::QBrightnessAndContrast(QQuickItem *parent)
    : QMatFilter(parent)
{
}

QBrightnessAndContrast::~QBrightnessAndContrast(){
}

void QBrightnessAndContrast::transform(const cv::Mat &in, cv::Mat &out){
    out.create(in.size(), in.type());
    for( int y = 0; y < in.rows; y++ ) {
        const uchar* pi = in.ptr<uchar>(y);
        uchar* po       = out.ptr<uchar>(y);

        for( int x = 0; x < in.cols; x++ ) {
            for( int c = 0; c < in.channels(); c++ ){
                int pos = x * in.channels() + c;
                po[pos] = cv::saturate_cast<uchar>(m_contrast * pi[pos] + m_brightness);
            }
        }
    }
}
