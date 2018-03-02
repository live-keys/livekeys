#include "qoverlapmat.h"

QOverlapMat::QOverlapMat(QQuickItem* parent)
    : QMatFilter(parent)
    , m_mask(QMat::nullMat())
    , m_input2(QMat::nullMat())
{
}

void QOverlapMat::transform(const cv::Mat &in, cv::Mat &out){
    if ( in.size() == m_mask->data().size() && in.size() == m_input2->data().size() ){
        in.copyTo(out);
        m_input2->data().copyTo(out, m_mask->data());
    }
}
