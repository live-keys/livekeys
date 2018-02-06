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

#include "qmat.h"
#include <QQmlEngine>


/*!
  \qmltype Mat
  \instantiates QMat
  \inqmlmodule lcvcore
  \inherits QObject
  \brief Custom matrix element.

  You can access a matrix's pixels from QML by using the buffer() function, which gives you a js ArrayBuffer. Here's a
  how you can access pixel values from a RGB matrix.

  \code
  ImRead{
      id : src
      file : project.path + '/sample.jpg'
      Component.onCompleted : {
           var buffer     = output.buffer()
           var size       = output.dimensions()
           var channels   = output.channels()

           var bufferview = new Uint8Array(buffer)

           for ( var i = 0; i < size.height; ++i ){
               for ( var j = 0; j < size.width; ++j ){
                   var b = bufferview[i * size.width + j * channels + 0]; // get the blue channel
                   var g = bufferview[i * size.width + j * channels + 1]; // get the green channel
                   var r = bufferview[i * size.width + j * channels + 2]; // get the red channel
               }
           }
       }
  }
  \endcode

  A sample on accessing and changing matrixes is available in \b{samples/core/customfilter.qml} :

  \quotefile core/customfilter.qml
*/


/*!
  \class QMat
  \inmodule lcvcore_cpp
  \brief Open cv matrix wrapper.

   The class represents the wrapper for the opencv matrix element to be passed around in the QML structure. To access
   its cv mat vaue, use the cvMat() function.

   To access it's pixels within qml, use the Mat::buffer() function.
 */

/*!
  \qmlproperty enumeration Mat::Type

  Matrix type. The type can be one of:
  \list
  \li Mat.CV8U
  \li Mat.CV8S
  \li Mat.CV16U
  \li Mat.CV16S
  \li Mat.CV32S
  \li Mat.CV32F
  \li Mat.CV64F
  \endlist
*/

/*!
  \enum QMat::Type

  \value CV8U
  \value CV8S
  \value CV16U
  \value CV16S
  \value CV32S
  \value CV32F
  \value CV64F
 */

/*!
  \brief QMat::QMat

  Params : \a parent
 */
QMat::QMat(QObject *parent):
    QObject(parent),
    m_cvmat(new cv::Mat){
}

/*!
  \brief QMat::QMat

  Params : \a mat , \a parent
 */
QMat::QMat(cv::Mat *mat, QObject *parent):
    QObject(parent),
    m_cvmat(mat){
}

/**
 * @brief Returns an equivalent ArrayBuffer to access the matrix values
 * @return
 */
QByteArray QMat::buffer(){
    return QByteArray::fromRawData(
        reinterpret_cast<const char*>(m_cvmat->data),
        static_cast<int>(m_cvmat->total() * m_cvmat->elemSize())
    );
}

/*!
  \qmlmethod int Mat::channels()

  Returns the number of channels of the matrix
*/

/**
 * @brief QMat::channels
 * @return
 */
int QMat::channels(){
    return m_cvmat->channels();
}

/**
 * @brief QMat::depth
 * @return
 */
int QMat::depth(){
    return m_cvmat->depth();
}

/*!
  \qmlmethod Size Mat::dimensions()

  Returns the size of the matrix
 */

/*!
  \brief Returns the size of the matrix element
 */
QSize QMat::dimensions() const{
    return QSize(m_cvmat->cols, m_cvmat->rows);
}


/*!
  \qmlmethod Mat Mat::createOwnedObject()
  \brief Returns a shallow copied matrix with javascript ownership
 */

/*!
  \brief Returns a shallow copied matrix that is owned by the javascript engine
 */
QMat* QMat::createOwnedObject(){
    cv::Mat* ownedObjectInternal = new cv::Mat(*m_cvmat);
    QMat*    ownedObject         = new QMat(ownedObjectInternal);
    QQmlEngine::setObjectOwnership(ownedObject, QQmlEngine::JavaScriptOwnership);
    return ownedObject;
}


/*!
  \qmlmethod Mat Mat::cloneMat()
  \brief Returns a cloned matrix with javascript ownership
 */

/*!
  \brief Returns a cloned matrix that is owned by the javascript engine
 */
QMat* QMat::cloneMat() const{
    cv::Mat* clonedMat = new cv::Mat;
    m_cvmat->copyTo(*clonedMat);
    QMat* clonedObject = new QMat(clonedMat);
    QQmlEngine::setObjectOwnership(clonedObject, QQmlEngine::JavaScriptOwnership);
    return clonedObject;
}

/*!
  \brief QMat::~QMat
 */
QMat::~QMat(){
    delete m_cvmat;
}

const cv::Mat &QMat::data() const{
    return *m_cvmat;
}

QMat* QMat::m_nullMat = 0;

/*!
 * \brief Returns a null matrix
 */
QMat*QMat::nullMat(){
    if ( !m_nullMat ){
        m_nullMat = new QMat;
        atexit(&QMat::cleanUp);
    }
    return m_nullMat;
}

/*!
 * \brief Internal method used to clean up the null matrix.
 */
void QMat::cleanUp(){
    delete m_nullMat;
}

/*!
 * \brief Deep clones the mat
 */
QMat *QMat::clone() const{
    cv::Mat* clonedMat = new cv::Mat;
    m_cvmat->copyTo(*clonedMat);
    QMat* clonedObject = new QMat(clonedMat);
    return clonedObject;
}

/*!
  \fn cv::Mat* QMat::cvMat()
  \brief Returns the contained open cv mat.
 */

