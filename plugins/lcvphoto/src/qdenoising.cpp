#include "qdenoising.h"
#include "live/shared.h"
#include "opencv2/photo.hpp"

QDenoising::QDenoising(QObject *parent)
    : QObject(parent)
{
}

void QDenoising::fastNlMeans(
        lv::Act *context,
        QMat *input,
        float h,
        int templateWindowSize,
        int searchWindowSize)
{
    QMat* m = new QMat;
    context->onRun(lv::Shared::refScope(context, input),[input, h, templateWindowSize, searchWindowSize, m](){
        try{
            cv::fastNlMeansDenoising(input->data(), *m->cvMat(), h, templateWindowSize, searchWindowSize);
        } catch ( cv::Exception& e ){
            qCritical("%s", e.msg.c_str());
        }
    },[m, context](){
        context->setResult(QVariant::fromValue(m));
    });
}

void QDenoising::fastNlMeansColored(
        lv::Act *context,
        QMat *input,
        float h,
        int templateWindowSize,
        int searchWindowSize)
{
    QMat* m = new QMat;

    context->onRun(lv::Shared::refScope(context, input), [input, h, templateWindowSize, searchWindowSize, m](){
        try{
            cv::fastNlMeansDenoising(input->data(), *m->cvMat(), h, templateWindowSize, searchWindowSize);
        } catch ( cv::Exception& e ){
            qCritical("%s", e.msg.c_str());
        }
    },[m, context](){
        context->setResult(QVariant::fromValue(m));
    });
}
