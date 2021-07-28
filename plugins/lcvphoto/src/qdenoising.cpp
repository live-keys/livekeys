#include "qdenoising.h"
#include "live/shared.h"
#include "qumat.h"
#include "opencv2/photo.hpp"

#include "live/exception.h"
#include "live/qmlerror.h"
#include "cvextras.h"
#include "qqmlapplicationengine.h"
#include "live/viewcontext.h"

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

QMat *QDenoising::denoiseTVL1(const QVariantList &obs, double lambda, int iters)
{
    if (obs.size() == 0) return nullptr;
    try {
        std::vector<cv::Mat> obsVector;
        for(int i = 0; i < obs.size(); i++)
        {
            QMat* mat = obs.at(i).value<QMat*>();
            if (!mat){
                return nullptr;
            }
            obsVector.push_back(mat->internal());
        }

        cv::Mat* rMat = new cv::Mat(
            obsVector[0].rows,
            obsVector[0].cols,
            CV_8UC1);
        cv::denoise_TVL1(obsVector, *rMat, lambda, iters);
        QMat* result = new QMat(rMat);

        return result;
    } catch (cv::Exception& e){
        lv::CvExtras::toLocalError(e, lv::ViewContext::instance().engine(), this, "Denoising: ").jsThrow();
        return nullptr;
    }
}
