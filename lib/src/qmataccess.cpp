/****************************************************************************
**
** Copyright (C) 2014-2016 Dinu SV.
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

#include "qmataccess.h"
#include <QElapsedTimer>

using namespace cv;

QElapsedTimer timer;

/*!
  \qmltype MatAccess
  \instantiates QMatAccess
  \inqmlmodule lcvcore
  \inherits QQuickItem
  \brief Accesses a matrixes pixel values.

  You can access a matrix's pixels from QML by using the data() function on a matrix element. The data() function
  returns a matrix data accessor type, which can then be used to access the properties of the selected matrix. Here's a
  how you can access pixel values from a RGB matrix.

  \code
  ImRead{
      id : src
      file : project.path + '/sample.jpg'
      Component.onCompleted : {
           var outputdata = output.data()
           var rows       = outputdata.rows()
           var cols       = outputdata.cols()
           var channels   = outputdata.channels()

           var vals = outputdata.values()

           for ( var i = 0; i < rows; ++i ){
               for ( var j = 0; j < cols; ++j ){
                   var b = vals[i][j * channels + 0]; // get the blue channel
                   var g = vals[i][j * channels + 1]; // get the green channel
                   var r = vals[i][j * channels + 2]; // get the red channel
                   }
               }
           }
       }
  }
  \endcode

  The rows(), cols() and channels() functions are self explanatory, while for the values() function it's important to
  note that it returns a 2D array of rows and columns, and that each of channels are merged into the same row.

  Changing a matrixes values can be done by changing the array returned by the values() function, and then setting it
  back by using the setValues() function.

  A sample on accessing and changing matrixes is available in \b{samples/core/customfilter.qml} :

  \quotefile core/customfilter.qml

 */

/*!
   \class QMatAccess
   \inmodule lcvlib_cpp
   \brief Used to access matrix pixel values in QML.
 */


namespace helpers{

template<typename T, typename LT> void cacheValues( Mat& m, QVariantList& output){
    T* data  = reinterpret_cast<T*>(m.data);
    int step = (int)(m.step / sizeof(T));

    for ( int i = 0; i < m.rows; ++i ){
        QList<LT> row;
        int rowstep = i * step;
        for ( int j = 0; j < m.cols * m.channels(); ++j ){
            row.append(data[rowstep + j]);
        }
        output.append(QVariant::fromValue(row));
    }
}

template<typename T, typename LT> void setValues( QVariantList& values, Mat& m){
    T* data  = reinterpret_cast<T*>(m.data);
    int step = (int)(m.step / sizeof(T));

    for ( int i = 0; i < m.rows; ++i ){
        QList<LT> row = values.at(i).value<QList<LT> >();
        int rowstep = i * step;
        for ( int j = 0; j < m.cols * m.channels(); ++j ){
            data[rowstep + j] = static_cast<T>(row.at(j));
        }
    }
}

}// namespace helpers

/*!
  \brief QMatAccess constructor
  \a parent
 */
QMatAccess::QMatAccess(QQuickItem *parent)
    : QQuickItem(parent)
    , m_mat(0){
}

/*!
  \brief QMatAccess constructor
  \a mat
  \a parent
 */
QMatAccess::QMatAccess(QMat *mat, QQuickItem *parent)
    : QQuickItem(parent)
    , m_mat(mat){

    Mat* cvmat = m_mat->cvMat();
    m_dataPtr  = cvmat->data;
    m_channels = cvmat->channels();

}

/*!
  \brief QMatAccess destructor
 */
QMatAccess::~QMatAccess(){
}

/*!
  \brief QMatAccess::setUpCache
 */
void QMatAccess::setupCache(){
    Mat* cvmat = m_mat->cvMat();

    switch(cvmat->depth()){
    case CV_8U:
        helpers::cacheValues<uchar, int>(*cvmat, m_values);
        break;
    case CV_8S:
        helpers::cacheValues<uchar, int>(*cvmat, m_values);
        break;
    case CV_16U:
        helpers::cacheValues<short, int>(*cvmat, m_values);
        break;
    case CV_16S:
        helpers::cacheValues<short, int>(*cvmat, m_values);
        break;
    case CV_32S:
        helpers::cacheValues<int, int>(*cvmat, m_values);
        break;
    case CV_32F:
        helpers::cacheValues<float, float>(*cvmat, m_values);
        break;
    case CV_64F:
        helpers::cacheValues<float, qreal>(*cvmat, m_values);
        break;
    }
}

/*!
  \qmlmethod int MatAccess::cols()

  Returns the number of columns.
 */

/*!
  \fn int QMatAccess::cols() const
  \brief Returns the number of columns
 */

/*!
  \qmlmethod int MatAccess::rows()

  Returns the number of rows.
 */

/*!
  \fn int QMatAccess::rows() const
  \brief Returns the number of rows
 */


/*!
  \qmlmethod int MatAccess::channels()

  Returns the number of channels.
 */

/*!
  \fn int QMatAccess::channels() const
  \brief Returns the number of channels
 */


/*!
  \qmlmethod int MatAccess::depth()

  Returns the depth of the matrix.
 */

/*!
  \fn int QMatAccess::depth() const
  \brief Returns the depth of the matrix
 */


/*!
  \qmlmethod variant MatAccess::values()

  Returns the values of the matrix as a multi-dimensional array.
 */

/*!
  \brief Returns the matrix data as a QVariantList.
 */
QVariantList QMatAccess::values(){
    if ( m_values.empty() )
        setupCache();
    return m_values;
}

/*!
  \qmlmethod MatAccess::setValues(variant)

  Sends a multi-dimensional array of processed values back to the matrix element.
 */

/*!
  \brief Set the matrix data from a QVariantList

  Parameters :
  \a values
 */
void QMatAccess::setValues(QVariantList values){
    m_values = values;

    Mat* cvmat = m_mat->cvMat();
    switch(cvmat->depth()){
    case CV_8U:
        helpers::setValues<uchar, int>(m_values, *cvmat);
        break;
    case CV_8S:
        helpers::setValues<uchar, int>(m_values, *cvmat);
        break;
    case CV_16U:
        helpers::setValues<short, int>(m_values, *cvmat);
        break;
    case CV_16S:
        helpers::setValues<short, int>(m_values, *cvmat);
        break;
    case CV_32S:
        helpers::setValues<int, int>(m_values, *cvmat);
        break;
    case CV_32F:
        helpers::setValues<float, float>(m_values, *cvmat);
        break;
    case CV_64F:
        helpers::setValues<float, qreal>(m_values, *cvmat);
        break;
    }
}

