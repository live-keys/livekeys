/****************************************************************************
**
** Copyright (C) 2014-2018 Dinu SV.
** (contact: mail@dinusv.com)
** This file is part of Live CV Application.
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

#include "qcvglobalobject.h"
#include "opencv2/core.hpp"

namespace helpers{

template<typename T, typename LT> void getValues( cv::Mat& m, QVariantList& output){
    T* data  = reinterpret_cast<T*>(m.data);
    int step = (int)(m.step / sizeof(T));

    for ( int i = 0; i < m.rows; ++i ){
        int rowstep = i * step;
        for ( int j = 0; j < m.cols * m.channels(); ++j ){
            output.append(data[rowstep + j]);
        }
    }
}

template<typename T> QMatrix4x4 matrix4x4( cv::Mat& m ){
    T* data  = reinterpret_cast<T*>(m.data);
    int step = (int)(m.step / sizeof(T));

    return QMatrix4x4(
        (float)data[step * 0 + 0], (float)data[step * 0 + 1], (float)data[step * 0 + 2], (float)data[step * 0 + 3],
        (float)data[step * 1 + 0], (float)data[step * 1 + 1], (float)data[step * 1 + 2], (float)data[step * 1 + 3],
        (float)data[step * 2 + 0], (float)data[step * 2 + 1], (float)data[step * 2 + 2], (float)data[step * 2 + 3],
        (float)data[step * 3 + 0], (float)data[step * 3 + 1], (float)data[step * 3 + 2], (float)data[step * 3 + 3]
    );
}

template<typename T, typename LT> void setValues(const QVariantList& values, cv::Mat& m){
    T* data  = reinterpret_cast<T*>(m.data);
    int step = (int)(m.step / sizeof(T));

    for ( int i = 0; i < m.rows; ++i ){
        int rowstep = i * step;
        for ( int j = 0; j < m.cols * m.channels(); ++j ){
            data[rowstep + j] =values.at(j + i * m.cols * m.channels()).value<T>();
        }
    }
}



} // namespace

QCvGlobalObject::QCvGlobalObject(QObject *parent)
    : QObject(parent){

}

QVariantList QCvGlobalObject::matToArray(QMat *m){
    QVariantList values;
    cv::Mat* cvmat = m->cvMat();
    switch(cvmat->depth()){
    case CV_8U:
        helpers::getValues<uchar, int>(*cvmat, values);
        break;
    case CV_8S:
        helpers::getValues<uchar, int>(*cvmat, values);
        break;
    case CV_16U:
        helpers::getValues<short, int>(*cvmat, values);
        break;
    case CV_16S:
        helpers::getValues<short, int>(*cvmat, values);
        break;
    case CV_32S:
        helpers::getValues<int, int>(*cvmat, values);
        break;
    case CV_32F:
        helpers::getValues<float, float>(*cvmat, values);
        break;
    case CV_64F:
        helpers::getValues<float, qreal>(*cvmat, values);
        break;
    }

    return values;
}

void QCvGlobalObject::assignArrayToMat(const QVariantList &a, QMat *m){
    cv::Mat* cvmat = m->cvMat();
    switch(cvmat->depth()){
    case CV_8U:
        helpers::setValues<uchar, int>(a, *cvmat);
        break;
    case CV_8S:
        helpers::setValues<uchar, int>(a, *cvmat);
        break;
    case CV_16U:
        helpers::setValues<short, int>(a, *cvmat);
        break;
    case CV_16S:
        helpers::setValues<short, int>(a, *cvmat);
        break;
    case CV_32S:
        helpers::setValues<int, int>(a, *cvmat);
        break;
    case CV_32F:
        helpers::setValues<float, float>(a, *cvmat);
        break;
    case CV_64F:
        helpers::setValues<float, qreal>(a, *cvmat);
        break;
    }
}

QMatrix4x4 QCvGlobalObject::matrix4x4(QMat *m){
    cv::Mat* cvmat = m->cvMat();

    if ( cvmat->rows == 4 && cvmat->cols == 4 && m->channels() == 1 ){

        switch(cvmat->depth()){
        case CV_8U: return helpers::matrix4x4<uchar>(*cvmat);
        case CV_8S: return helpers::matrix4x4<uchar>(*cvmat);
        case CV_16U: return helpers::matrix4x4<ushort>(*cvmat);
        case CV_16S: return helpers::matrix4x4<short>(*cvmat);
        case CV_32S: return helpers::matrix4x4<int>(*cvmat);
        case CV_32F: return helpers::matrix4x4<float>(*cvmat);
        case CV_64F: return helpers::matrix4x4<double>(*cvmat);
        }
    }
    qWarning("Matrix not of 4x4 and single channel type.");
    return QMatrix4x4();
}
