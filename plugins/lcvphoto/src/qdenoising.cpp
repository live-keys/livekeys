#include "qdenoising.h"
#include "live/shared.h"
#include "qumat.h"
#include "opencv2/photo.hpp"

#include "live/exception.h"
#include "live/qmlerror.h"

QDenoising::QDenoising(QObject *parent)
    : QObject(parent)
{
}

QObject *QDenoising::fastNlMeans(
        QObject *input,
        float h,
        int templateWindowSize,
        int searchWindowSize)
{
    QMat* inputMat = qobject_cast<QMat*>(input);
    if ( inputMat ){
        QMat* m = new QMat;
        try{
            cv::fastNlMeansDenoising(inputMat->internal(), *m->internalPtr(), h, templateWindowSize, searchWindowSize);
        } catch ( cv::Exception& e ){
            THROW_QMLERROR(lv::Exception, "MatOp: " + e.msg, lv::Exception::toCode("OpenCV"), this);
            delete m;
            return nullptr;
        }

        return m;
    }

    QUMat* inputUMat = qobject_cast<QUMat*>(input);
    if ( inputUMat ){
        QUMat* um = new QUMat;

        try{
            cv::fastNlMeansDenoising(inputUMat->internal(), *um->internalPtr(), h, templateWindowSize, searchWindowSize);
        } catch ( cv::Exception& e ){
            THROW_QMLERROR(lv::Exception, "MatOp: " + e.msg, lv::Exception::toCode("OpenCV"), this);
            delete um;
            return nullptr;
        }

        return um;
    }

    return nullptr;
}

QObject* QDenoising::fastNlMeansColored(
        QObject *input,
        float h,
        int templateWindowSize,
        int searchWindowSize)
{
    QMat* inputMat = qobject_cast<QMat*>(input);
    if ( inputMat ){
        QMat* m = new QMat;
        try{
            cv::fastNlMeansDenoising(inputMat->internal(), *m->internalPtr(), h, templateWindowSize, searchWindowSize);
        } catch ( cv::Exception& e ){
            THROW_QMLERROR(lv::Exception, "MatOp: " + e.msg, lv::Exception::toCode("OpenCV"), this);
            delete m;
            return nullptr;
        }

        return m;
    }

    QUMat* inputUMat = qobject_cast<QUMat*>(input);
    if ( inputUMat ){
        QUMat* um = new QUMat;

        try{
            cv::fastNlMeansDenoising(inputUMat->internal(), *um->internalPtr(), h, templateWindowSize, searchWindowSize);
        } catch ( cv::Exception& e ){
            THROW_QMLERROR(lv::Exception, "MatOp: " + e.msg, lv::Exception::toCode("OpenCV"), this);
            delete um;
            return nullptr;
        }

        return um;
    }

    return nullptr;
}
