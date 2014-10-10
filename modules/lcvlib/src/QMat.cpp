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

/*!
  \qmltype Mat
  \instantiates QMat
  \inqmlmodule lcvcore
  \inherits QQuickItem
  \brief The main element passed around in the QML structure.
*/


/*!
  \class QMat
  \inmodule lcvcore_cpp
  \brief Open cv matrix wrapper.

   The class represents the wrapper for the opencv matrix element to be passed around in the QML structure. To access
   its cv mat vaue, use the cvMat() function.

   To access it's pixels within qml, use the Mat::data() function.
 */


/*!
  \brief QMat::QMat
  \a parent
 */
QMat::QMat(QQuickItem *parent):
    QQuickItem(parent),
    m_cvmat(new cv::Mat){
}

/*!
  \brief QMat::QMat
  \a mat
  \a parent
 */
QMat::QMat(cv::Mat *mat, QQuickItem *parent):
    QQuickItem(parent),
    m_cvmat(mat){
}

/*!
  \qmlmethod MatAccess Mat::data()

  Retruns a matrix accessor element to access it's pixels.
 */

/*!
  \brief QMat::data
  \return
 */
QMatAccess *QMat::data(){
    return new QMatAccess(this);
}

/*!
  \brief QMat::~QMat
 */
QMat::~QMat(){
    delete m_cvmat;
}
