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
#include "QCvtColor.hpp"
#include "opencv2/imgproc/imgproc.hpp"

using namespace cv;

/*!
  \qmltype CvtColor
  \instantiates QCvtColor
  \inqmlmodule lcvimgproc
  \inherits MatFilter
  \brief

  \quotefile imgproc/blur.qml
*/

/*!
  \class QCvtColor
  \inmodule lcvimgproc_cpp
  \brief
 */

/*!
  \brief QCvtColor constructor

  Parameters:
  \a parent
 */
QCvtColor::QCvtColor(QQuickItem *parent) :
    QMatFilter(parent)
{
}

/*!
  \property QCvtColor::code
  \sa CvtColor::code
 */

/*!
  \qmlproperty CvtColor::CvtType CvtColor::code

  Color space conversion code.
 */

/*!
  \property QCvtColor::dstCn
  \sa CvtColor::dstCn
 */

/*!
  \qmlproperty int CvtColor::dstCn

  Number of channels in the destination image; if the parameter is 0, the number of the channels is derived
  automatically from input and code.
 */


/*!
  \brief Filtering function.

  Parameters:
  \a in
  \a out
 */
void QCvtColor::transform(cv::Mat &in, cv::Mat &out){
    cvtColor(in, out, m_code, m_dstCn);
}
