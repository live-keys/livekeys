#include "qfeaturedetection.h"
#include "cvextras.h"
#include "live/viewcontext.h"

QFeatureDetection::QFeatureDetection(QObject *parent) : QObject(parent)
{

}

QMat *QFeatureDetection::houghLines(QMat* input, double rho, double theta, int threshold,
                                    double srn, double stn)
{
    if (!input || input->internal().empty()) return nullptr;
    try{
        cv::Mat* rMat = new cv::Mat(
            input->dimensions().height(),
            input->dimensions().width(),
            CV_MAKETYPE(input->depth(), input->channels()));
        cv::HoughLines(input->internal(), *rMat, rho, theta, threshold, srn, stn);
        return new QMat(rMat);

    } catch (cv::Exception& e){
        lv::CvExtras::toLocalError(e, lv::ViewContext::instance().engine(), this, "FeatureDetection: ").jsThrow();
        return nullptr;
    }
}

QMat *QFeatureDetection::houghLinesP(QMat *input, double rho, double theta, int threshold, double minLineLength, double maxLineGap)
{
    if (!input || input->internal().empty()) return nullptr;
    try{
        cv::Mat* rMat = new cv::Mat(
            input->dimensions().height(),
            input->dimensions().width(),
            CV_MAKETYPE(input->depth(), input->channels()));
        cv::HoughLinesP(input->internal(), *rMat, rho, theta, threshold, minLineLength, maxLineGap);
        return new QMat(rMat);

    } catch (cv::Exception& e){
        lv::CvExtras::toLocalError(e, lv::ViewContext::instance().engine(), this, "FeatureDetection: ").jsThrow();
        return nullptr;
    }
}
