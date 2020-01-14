#include "qdenoising.h"
#include "live/shared.h"
#include "opencv2/photo.hpp"

QDenoising::QDenoising(QObject *parent)
    : QObject(parent)
{
}

QMat* QDenoising::fastNlMeans(
        QMat *input,
        float h,
        int templateWindowSize,
        int searchWindowSize)
{
    QMat* m = new QMat;
    try{
        cv::fastNlMeansDenoising(input->data(), *m->cvMat(), h, templateWindowSize, searchWindowSize);
    } catch ( cv::Exception& e ){
        qCritical("%s", e.msg.c_str());
    }
    return m;
}

QMat* QDenoising::fastNlMeansColored(
        QMat *input,
        float h,
        int templateWindowSize,
        int searchWindowSize)
{
    QMat* m = new QMat;
    try{
        cv::fastNlMeansDenoising(input->data(), *m->cvMat(), h, templateWindowSize, searchWindowSize);
    } catch ( cv::Exception& e ){
        qCritical("%s", e.msg.c_str());
    }
    return m;
}
