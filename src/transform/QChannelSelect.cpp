#include "QChannelSelect.hpp"
#include "opencv2/imgproc/imgproc.hpp"

QChannelSelect::QChannelSelect(QQuickItem *parent) :
    QMatTransformation(parent)
{
}

QChannelSelect::~QChannelSelect(){
}

void QChannelSelect::transform(cv::Mat &in, cv::Mat &out){
    if ( out.channels() == 3 )
        cv::cvtColor(out, out, CV_BGR2GRAY);
    if ( !in.empty() ){
        if ( in.channels() == 1 ){
            in.copyTo(out);
            return;
        }
        std::vector<cv::Mat> channels(3);
        cv::split(in, channels);
        channels[m_channel].copyTo(out);
    }
}
