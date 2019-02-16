#include "qdenoising.h"
#include "live/shared.h"
#include "opencv2/photo.hpp"

QDenoising::QDenoising(QObject *parent)
    : QObject(parent)
{
}

void QDenoising::fastNlMeansAsync(
        lv::Filter *context,
        QMat *input,
        float h,
        int templateWindowSize,
        int searchWindowSize,
        const QJSValue &allocator)
{
    lv::Shared::ReadScope* rs = lv::Shared::readScope(context, input);
    if ( rs->reserved() ){
        QMat* m = new QMat;

        context->use(lv::Shared::readScope(context, input),[input, h, templateWindowSize, searchWindowSize, allocator, &m](){
            cv::fastNlMeansDenoising(input->data(), *m->cvMat(), h, templateWindowSize, searchWindowSize);
        },[&m](){
            //TODO: Update context
        });

    } else {
        delete rs;
    }
}

void QDenoising::fastNlMeansColoredAsync(
        lv::Filter *context,
        QMat *input,
        float h,
        int templateWindowSize,
        int searchWindowSize,
        const QJSValue &allocator)
{
    lv::Shared::ReadScope* rs = lv::Shared::readScope(context, input);
    if ( rs->reserved() ){
        QMat* m = new QMat;

        context->use(lv::Shared::readScope(context, input),[input, h, templateWindowSize, searchWindowSize, allocator, &m](){
            cv::fastNlMeansDenoising(input->data(), *m->cvMat(), h, templateWindowSize, searchWindowSize);
        },[&m](){
            //TODO: Update context
        });

    } else {
        delete rs;
    }
}
