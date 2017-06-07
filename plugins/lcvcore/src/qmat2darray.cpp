/****************************************************************************
**
** Copyright (C) 2014-2017 Dinu SV.
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

#include "qmat2darray.h"

using namespace cv;

/*!
  \qmltype Mat2DArray
  \instantiates QMat2DArray
  \inqmlmodule lcvcore
  \inherits MatDisplay
  \brief Creates a mat from a 2D javascript array.

  Usage available under \b{samples/core/mat2darray.qml}

  \quotefile core/mat2darray.qml
*/

/*!
   \class QMat2DArray
   \internal
   \brief Creates a mat from a 2D javascript array.
 */

template<typename T> cv::Size assignValuesHelper(const QVariantList& vmat, Mat* m, QMat::Type type){
    int numberCols = 0;
    for( int i = 0; i < vmat.size(); ++i ){
        if ( !vmat[i].canConvert(QMetaType::QVariantList) ){
            qWarning("Open CV Error: Values must me stored in a 2D array.");
            m->create(0, 0, CV_MAKETYPE(type, 1));
            return cv::Size(0, 0);
        }
        QVariantList row = vmat[i].toList();
        if ( i == 0 ){
            numberCols = row.size();
            m->create(vmat.size(), numberCols, CV_MAKETYPE(type, 1));
            m->setTo(Scalar(0));
        }
        T* mdata = m->ptr<T>(i);
        for( int j = 0; j < numberCols; ++j ){
            if ( j < row.size() )
                mdata[j] = row[j].value<T>();
            else
                mdata[j] = 0;
        }
    }
    return cv::Size(numberCols, vmat.size());
}

QMat2DArray::QMat2DArray(QQuickItem *parent)
    : QMatDisplay(parent)
    , m_type(QMat::CV8U)
{
}

QMat2DArray::~QMat2DArray(){
}

/*!
  \qmlproperty variant Mat2DArray::values

  The values in js 2d array form.
 */

/*!
  \qmlproperty Mat::Type Mat2DArray::type

  The type of the matrix.
 */

cv::Size QMat2DArray::assignValues(){
    switch(m_type){
    case CV_8U :  return assignValuesHelper<unsigned char>(  m_values, output()->cvMat(), m_type);
    case CV_8S :  return assignValuesHelper<char>(           m_values, output()->cvMat(), m_type);
    case CV_16U : return assignValuesHelper<unsigned short>( m_values, output()->cvMat(), m_type);
    case CV_16S : return assignValuesHelper<short>(          m_values, output()->cvMat(), m_type);
    case CV_32S : return assignValuesHelper<long>(           m_values, output()->cvMat(), m_type);
    case CV_32F : return assignValuesHelper<float>(          m_values, output()->cvMat(), m_type);
    case CV_64F : return assignValuesHelper<double>(         m_values, output()->cvMat(), m_type);
    }
    return cv::Size(0, 0);
}
