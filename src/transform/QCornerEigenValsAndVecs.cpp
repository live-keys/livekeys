#include "QCornerEigenValsAndVecs.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"

using namespace cv;

double myHarris_minVal; double myHarris_maxVal;
double myShiTomasi_minVal; double myShiTomasi_maxVal;
int myShiTomasi_qualityLevel = 50;
int myHarris_qualityLevel = 50;
int max_qualityLevel = 100;

cv::RNG rng(12345);

QCornerEigenValsAndVecs::QCornerEigenValsAndVecs(QQuickItem *parent) :
    QMatFilter(parent),
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
    else {
        m_mc = cv::Mat::zeros( out.size(), CV_32FC1 );
        cv::cornerEigenValsAndVecs(out, out, m_blockSize, m_ksize, m_borderType);

        /* calculate Mc */
        for( int j = 0; j < out.rows; j++ )
           { for( int i = 0; i < out.cols; i++ )
                {
                  float lambda_1 = out.at<cv::Vec6f>(j, i)[0];
                  float lambda_2 = out.at<cv::Vec6f>(j, i)[1];
                  m_mc.at<float>(j,i) = lambda_1*lambda_2 - 0.04f*pow( ( lambda_1 + lambda_2 ), 2 );
                }
           }

        cv::minMaxLoc( m_mc, &myHarris_minVal, &myHarris_maxVal, 0, 0, cv::Mat() );

        cv::Mat myHarris_copy(out.size(), CV_8UC3, cv::Scalar(0));// = out.clone();

        if( myHarris_qualityLevel < 1 ) { myHarris_qualityLevel = 1; }

        for( int j = 0; j < out.rows; j++ ){
            for( int i = 0; i < out.cols; i++ ){
                if (
                    m_mc.at<float>(j,i) > myHarris_minVal + ( myHarris_maxVal - myHarris_minVal ) * myHarris_qualityLevel / max_qualityLevel ){
                      cv::circle( myHarris_copy, cv::Point(i,j), 4, cv::Scalar( rng.uniform(0,255), rng.uniform(0,255), rng.uniform(0,255) ), -1, 8, 0 );
                }
            }
        }
        myHarris_copy.assignTo(out);
        //cv::imshow( "lalalal", myHarris_copy );
    }
}
