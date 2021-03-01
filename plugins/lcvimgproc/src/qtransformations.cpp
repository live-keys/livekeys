#include "qtransformations.h"
#include "opencv2/imgproc.hpp"

QTransformations::QTransformations(QObject *parent) : QObject(parent)
{

}

QMat* QTransformations::threshold(QMat* input, double thresh, double maxVal, int type)
{
    if (!input) return nullptr;
    QMat* output = new QMat(input->dimensions().width(), input->dimensions().height(), QMat::CV8U, 3);
    cv::threshold(input->internal(), output->internal(), thresh, maxVal, type);
    return output;
}

QMat *QTransformations::blend(QMat *src1, QMat *src2, QMat *mask)
{
    if (!src1 || !src2 || !mask) return nullptr;
    if (src1->dimensions() != src2->dimensions()) return nullptr;
    if (src1->dimensions() != mask->dimensions()) return nullptr;

    QMat* gray = new QMat(mask->dimensions().width(), mask->dimensions().height(), QMat::CV8U, 1);
    cv::cvtColor(mask->internal(), gray->internal(), cv::COLOR_BGR2GRAY);

    QMat* result = new QMat(mask->dimensions().width(), mask->dimensions().height(), QMat::CV8U, 3);
    auto intRes = result->internal();
    auto intSrc1 = src1->internal();
    auto intSrc2 = src2->internal();
    auto intMask = gray->internal();

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
                po[j * 3 + 0] = pi1[j * 3 + 0] * alpha + pi2[j * 3 + 0] * (1.0-alpha);
                po[j * 3 + 1] = pi1[j * 3 + 1] * alpha + pi2[j * 3 + 1] * (1.0-alpha);
                po[j * 3 + 2] = pi1[j * 3 + 2] * alpha + pi2[j * 3 + 2] * (1.0-alpha);
            }
        }
    }

    return result;
}
