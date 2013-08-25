#include "QCanny.hpp"
#include "opencv2/imgproc/imgproc.hpp"

QCanny::QCanny(QQuickItem *parent) :
    QMatTransformation(parent),
    m_apertureSize(3),
    m_l2gradient(false){
}

void QCanny::transform(cv::Mat &in, cv::Mat &out){
    cv::Canny(in, out, m_threshold1, m_threshold2, m_apertureSize, m_l2gradient);

    /*if ( in.channels() == 3 )
        cv::cvtColor(in, out, CV_BGR2GRAY);
    if ( !in.empty() ){
        if ( in.channels() == 1 ){
            in.copyTo(out);
            return;
        }
        std::vector<cv::Mat> channels(3);
        cv::split(in, channels);
        channels[m_channel].copyTo(out);
    }*/
}
