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
        int searchWindowSize,
        const QJSValue &allocator)
{
    lv::Shared::ReadScope* rs = lv::Shared::readScope(context, input);
    if ( rs->reserved() ){
        QMat* m = new QMat;

        context->use(rs,[input, h, templateWindowSize, searchWindowSize, allocator, &m](){
            try{
                cv::fastNlMeansDenoising(input->data(), *m->cvMat(), h, templateWindowSize, searchWindowSize);
            } catch ( cv::Exception& e ){
                qCritical("%s", e.msg.c_str());
            }
        },[&m, context](){
            context->setResult(QVariant::fromValue(m));
        });

    } else {
        delete rs;
    }
}

void QDenoising::fastNlMeansColored(
        lv::Act *context,
        QMat *input,
        float h,
        int templateWindowSize,
        int searchWindowSize,
        const QJSValue &allocator)
{
    lv::Shared::ReadScope* rs = lv::Shared::readScope(context, input);
    if ( rs->reserved() ){
        QMat* m = new QMat;

        context->use(rs,[input, h, templateWindowSize, searchWindowSize, allocator, &m](){
            try{
                cv::fastNlMeansDenoising(input->data(), *m->cvMat(), h, templateWindowSize, searchWindowSize);
            } catch ( cv::Exception& e ){
                qCritical("%s", e.msg.c_str());
            }
        },[&m, context](){
            context->setResult(QVariant::fromValue(m));
        });

    } else {
        delete rs;
    }
}
