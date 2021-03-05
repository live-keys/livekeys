#include "qtransformations.h"
#include "opencv2/imgproc.hpp"
#include "cvextras.h"
#include "qqmlapplicationengine.h"
#include "live/viewcontext.h"

QTransformations::QTransformations(QObject *parent) : QObject(parent)
{

}

QMat* QTransformations::threshold(QMat* input, double thresh, double maxVal, int type)
{
    if (!input)
        return nullptr;
    try {
        QMat* output = new QMat(input->dimensions().width(), input->dimensions().height(), QMat::CV8U, 3);
        cv::threshold(input->internal(), output->internal(), thresh, maxVal, type);
        return output;
    } catch (cv::Exception& e){
        lv::CvExtras::toLocalError(e, lv::ViewContext::instance().engine(), this, "Transformations: ").jsThrow();
    }
}
QMat *QTransformations::blend(QMat *src1, QMat *src2, QMat *mask)
{

    if (!src1 && src2) return src2;
    if (src1 && !src2) return src1;
    if ((!src1 && !src2) || !mask) return nullptr;
    if (src1->dimensions() != src2->dimensions()) return nullptr;
    if (src1->dimensions() != mask->dimensions()) return nullptr;

    try {
        cv::Mat intMask;
        if (mask->channels() == 1)
            intMask = mask->internal();
        else
            cv::cvtColor(mask->internal(), intMask, cv::COLOR_BGR2GRAY);

        int src1chan = src1->channels();
        int src2chan = src2->channels();
        int reschan = qMax(src1chan, src2chan);

        QMat* result = new QMat(mask->dimensions().width(), mask->dimensions().height(), QMat::CV8U, reschan);
        auto intRes = result->internal();
        auto intSrc1 = src1->internal();
        auto intSrc2 = src2->internal();

        const uchar* pi1, *pi2;
        uchar *pm, *po;
        {
            for ( int i = 0; i < intRes.rows; ++i ){
                pi1 = intSrc1.ptr<uchar>(i);
                pi2 = intSrc2.ptr<uchar>(i);
                pm = intMask.ptr<uchar>(i);
                po = intRes.ptr<uchar>(i);
                for ( int j = 0; j < intRes.cols; ++j ){
                    double alpha = pm[j]/ 255.0;
                    po[j * reschan + 0] = pi1[j * src1chan + 0] * alpha + pi2[j * src2chan + 0] * (1.0-alpha);
                    po[j * reschan + 1] = pi1[j * src1chan + 1] * alpha + pi2[j * src2chan + 1] * (1.0-alpha);
                    po[j * reschan + 2] = pi1[j * src1chan + 2] * alpha + pi2[j * src2chan + 2] * (1.0-alpha);
                    if (reschan == 4){

                        po[j * 4 + 3] = (src1chan == 4 ? pi1[j * 4 + 3] : 255) * alpha
                                      + (src2chan == 4 ? pi2[j * 4 + 3] : 255) * (1.0-alpha);
                    }
                }
            }
        }

        return result;
    } catch (cv::Exception& e){
        lv::CvExtras::toLocalError(e, lv::ViewContext::instance().engine(), this, "Transformations: ").jsThrow();
    }
}
