/****************************************************************************
**
** Copyright (C) 2022 Dinu SV.
** This file is part of Livekeys Application.
**
** GNU Lesser General Public License Usage
** This file may be used under the terms of the GNU Lesser
** General Public License version 3 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPLv3 included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 3 requirements
** will be met: https://www.gnu.org/licenses/lgpl.html.
**
****************************************************************************/

#include "qfilteringoperations.h"
#include "cvextras.h"
#include "live/viewcontext.h"

QFilteringOperations::QFilteringOperations(QObject* parent)
    : QObject(parent)
{

}

QMat *QFilteringOperations::blur(QMat *input, QSize size, QPoint point, int borderType)
{
    if (!input || input->internal().empty()) return nullptr;
    try{
        cv::Mat* rMat = new cv::Mat(
            input->dimensions().height(),
            input->dimensions().width(),
            CV_MAKETYPE(input->depth(), input->channels()));
        cv::blur(input->internal(), *rMat, cv::Size(size.width(), size.height()), cv::Point(point.x(), point.y()), borderType);
        QMat* result = new QMat(rMat);
        return result;

    } catch (cv::Exception& e){
        lv::CvExtras::toLocalError(e, lv::ViewContext::instance().engine(), this, "FilteringOperations: ").jsThrow();
        return nullptr;
    }

}

QMat *QFilteringOperations::canny(QMat *input, double threshold1, double threshold2, int apertureSize, bool L2gradient)
{
    if (!input || input->internal().empty()) return nullptr;
    try{
        cv::Mat* rMat = new cv::Mat(
            input->dimensions().height(),
            input->dimensions().width(),
            CV_MAKETYPE(input->depth(), input->channels()));
        cv::Canny(input->internal(), *rMat, threshold1, threshold2, apertureSize, L2gradient);
        QMat* result = new QMat(rMat);
        return result;

    } catch (cv::Exception& e){
        lv::CvExtras::toLocalError(e, lv::ViewContext::instance().engine(), this, "FilteringOperations: ").jsThrow();
        return nullptr;
    }
}

QMat *QFilteringOperations::dilate(QMat *input, QMat *kernel, QPoint anchor, int iterations, int borderType, QColor borderValue)
{
    if (!input || input->internal().empty() || !kernel || kernel->internal().empty()) return nullptr;
    try{
        cv::Mat* rMat = new cv::Mat(
            input->dimensions().height(),
            input->dimensions().width(),
            CV_MAKETYPE(input->depth(), input->channels()));
        cv::Scalar bv = cv::morphologyDefaultBorderValue();
        if ( borderValue.isValid() ){
            if ( input->channels() == 3 )
                bv = cv::Scalar(borderValue.blue(), borderValue.green(), borderValue.red());
            else
                bv = cv::Scalar(borderValue.red());
        }
        cv::dilate(input->internal(),
                   *rMat, kernel->internal(),
                   cv::Point(anchor.x(), anchor.y()),
                   iterations,
                   borderType,
                   bv);
        QMat* result = new QMat(rMat);
        return result;

    } catch (cv::Exception& e){
        lv::CvExtras::toLocalError(e, lv::ViewContext::instance().engine(), this, "FilteringOperations: ").jsThrow();
        return nullptr;
    }
}

QMat *QFilteringOperations::erode(QMat *input, QMat *kernel, QPoint anchor, int iterations, int borderType, QColor borderValue)
{
    if (!input || input->internal().empty() || !kernel || kernel->internal().empty()) return nullptr;
    try{
        cv::Mat* rMat = new cv::Mat(
            input->dimensions().height(),
            input->dimensions().width(),
            CV_MAKETYPE(input->depth(), input->channels()));
        cv::Scalar bv = cv::morphologyDefaultBorderValue();
        if ( borderValue.isValid() ){
            if ( input->channels() == 3 )
                bv = cv::Scalar(borderValue.blue(), borderValue.green(), borderValue.red());
            else
                bv = cv::Scalar(borderValue.red());
        }
        cv::erode(input->internal(),
                   *rMat, kernel->internal(),
                   cv::Point(anchor.x(), anchor.y()),
                   iterations,
                   borderType,
                   bv);
        QMat* result = new QMat(rMat);
        return result;

    } catch (cv::Exception& e){
        lv::CvExtras::toLocalError(e, lv::ViewContext::instance().engine(), this, "FilteringOperations: ").jsThrow();
        return nullptr;
    }
}

QMat *QFilteringOperations::filter2D(QMat *input, int ddepth, QMat *kernel, QPoint anchor, double delta, int borderType)
{
    if (!input || input->internal().empty() || !kernel || kernel->internal().empty()) return nullptr;
    try{
        cv::Mat* rMat = new cv::Mat(
            input->dimensions().height(),
            input->dimensions().width(),
            CV_MAKETYPE(input->depth(), input->channels()));
        cv::filter2D(input->internal(),
                     *rMat,
                     ddepth,
                     kernel->internal(),
                     cv::Point(anchor.x(), anchor.y()),
                     delta, borderType);
        QMat* result = new QMat(rMat);
        return result;

    } catch (cv::Exception& e){
        lv::CvExtras::toLocalError(e, lv::ViewContext::instance().engine(), this, "FilteringOperations: ").jsThrow();
        return nullptr;
    }
}

QMat *QFilteringOperations::gaussianBlur(QMat *input, QSize size, double sigmaX, double sigmaY, int borderType)
{
    if (!input || input->internal().empty()) return nullptr;
    try{
        cv::Mat* rMat = new cv::Mat(
            input->dimensions().height(),
            input->dimensions().width(),
            CV_MAKETYPE(input->depth(), input->channels()));
        cv::GaussianBlur(input->internal(), *rMat, cv::Size(size.width(), size.height()), sigmaX, sigmaY, borderType);
        QMat* result = new QMat(rMat);
        return result;

    } catch (cv::Exception& e){
        lv::CvExtras::toLocalError(e, lv::ViewContext::instance().engine(), this, "FilteringOperations: ").jsThrow();
        return nullptr;
    }
}

QMat *QFilteringOperations::sobel(QMat *input, int ddepth, int xorder, int yorder, int ksize, double scale, double delta, int borderType)
{
    if (!input || input->internal().empty()) return nullptr;
    try{
        cv::Mat* rMat = new cv::Mat(
            input->dimensions().height(),
            input->dimensions().width(),
            CV_MAKETYPE(input->depth(), input->channels()));
        cv::Sobel(input->internal(), *rMat, ddepth, xorder, yorder, ksize, scale, delta, borderType);
        QMat* result = new QMat(rMat);
        return result;

    } catch (cv::Exception& e){
        lv::CvExtras::toLocalError(e, lv::ViewContext::instance().engine(), this, "FilteringOperations: ").jsThrow();
        return nullptr;
    }
}

QMat *QFilteringOperations::getStructuringElement(int shape, QSize size, QPoint anchor)
{
    try{
        cv::Mat mat = cv::getStructuringElement(shape,
                                                cv::Size(size.width(), size.height()),
                                                cv::Point(anchor.x(), anchor.y()));
        QMat* result = new QMat();
        *result->internalPtr() = mat;
        return result;

    } catch (cv::Exception& e){
        lv::CvExtras::toLocalError(e, lv::ViewContext::instance().engine(), this, "FilteringOperations: ").jsThrow();
        return nullptr;
    }
}
