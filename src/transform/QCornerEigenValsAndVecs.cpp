#include "QCornerEigenValsAndVecs.hpp"
#include "opencv2/imgproc/imgproc.hpp"

QCornerEigenValsAndVecs::QCornerEigenValsAndVecs(QQuickItem *parent) :
    QMatTransformation(parent),
    m_borderType(cv::BORDER_DEFAULT){
}

void QCornerEigenValsAndVecs::transform(cv::Mat &in, cv::Mat &out){
    if ( in.channels() == 3 )
        cv::cvtColor(in, out, CV_BGR2GRAY);
    else {
        out = in;
    }
    if ( m_ksize == 0 || m_ksize > 31 || m_ksize % 2 == 0 || m_blockSize == 0 )
        qDebug() << "Error[CornerEigenValsAndVecs] : One of arguments' values is out of range "
                    "(The kernel size must be odd and not larger than 31)";
    else
        cv::cornerEigenValsAndVecs(out, out, m_blockSize, m_ksize, m_borderType);
}
