/****************************************************************************
**
** Copyright (C) 2014 Dinu SV.
** (contact: mail@dinusv.com)
** This file is part of Live CV application.
**
** GNU General Public License Usage
** 
** This file may be used under the terms of the GNU General Public License 
** version 3.0 as published by the Free Software Foundation and appearing 
** in the file LICENSE.GPL included in the packaging of this file.  Please 
** review the following information to ensure the GNU General Public License 
** version 3.0 requirements will be met: http://www.gnu.org/copyleft/gpl.html.
**
****************************************************************************/

#include "QMat.hpp"
#include "QMatAccess.hpp"
#include <QQmlEngine>


/*!
  \qmltype Mat
  \instantiates QMat
  \inqmlmodule lcvcore
  \inherits QQuickItem
  \brief The main element passed around in the QML structure.
*/


/*!
  \class QMat
  \inmodule lcvlib_cpp
  \brief Open cv matrix wrapper.

   The class represents the wrapper for the opencv matrix element to be passed around in the QML structure. To access
   its cv mat vaue, use the cvMat() function.

   To access it's pixels within qml, use the Mat::data() function.
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
QMat::QMat(QQuickItem *parent):
    QQuickItem(parent),
    m_cvmat(new cv::Mat){
}

/*!
  \brief QMat::QMat

  Params : \a mat , \a parent
 */
QMat::QMat(cv::Mat *mat, QQuickItem *parent):
    QQuickItem(parent),
    m_cvmat(mat){
}

/*!
  \qmlmethod MatAccess Mat::data()

  Returns a matrix accessor element to access the matrixes pixels.
 */

/*!
  \brief Retruns the matrix access element.
 */
QMatAccess *QMat::data(){
    return new QMatAccess(this);
}

/*!
  \qmlmethod Size Mat::dataSize()

  Returns the size of the internal matrix
 */

/*!
  \brief Returns the size of the matrix element
 */
QSize QMat::dataSize() const{
    return QSize(m_cvmat->cols, m_cvmat->rows);
}

QMat* QMat::createOwnedObject(){
    cv::Mat* ownedObjectInternal = new cv::Mat(*m_cvmat);
    QMat*    ownedObject         = new QMat(ownedObjectInternal);
    QQmlEngine::setObjectOwnership(ownedObject, QQmlEngine::JavaScriptOwnership);
    return ownedObject;
}

/*!
  \brief QMat::~QMat
 */
QMat::~QMat(){
    delete m_cvmat;
}

/*!
  \fn cv::Mat* QMat::cvMat()
  \brief Returns the contained open cv mat.
 */

