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
#include "qsobel.h"
#include "opencv2/imgproc/imgproc.hpp"
#include "qmatnode.h"
#include <QSGSimpleMaterial>

using namespace cv;

/*!
  \qmltype Sobel
  \instantiates QSobel
  \inqmlmodule lcvimgproc
  \inherits MatFilter
  \brief Calculates the first, second, third, or mixed image derivatives using an extended Sobel operator.

  \quotefile imgproc/sobeloperator.qml
*/

/*!
  \class QSobel
  \inmodule lcvimgproc_cpp
  \brief Calculates the first, second, third, or mixed image derivatives using an extended Sobel operator.
 */

/*!
  \brief QSobel constructor

  Parameters:
  \a parent
 */
QSobel::QSobel(QQuickItem *parent)
    : QMatFilter(parent)
    , m_display(new QMat)
    , m_ddepth(QMat::CV16S)
    , m_xorder(1)
    , m_yorder(0)
    , m_ksize(3)
    , m_scale(1)
    , m_delta(0)
    , m_borderType(BORDER_DEFAULT){
}

/*!
  \brief QSobel destructor
 */
QSobel::~QSobel(){
    delete m_display;
}


/*!
  \property QSobel::ddepth
  \sa Sobel::ddepth
 */

/*!
  \qmlproperty Mat::Type Sobel::ddepth

  Output image depth, the following combinations of input depth and ddepth are supproted:

  \list
  \li input.depth = Mat.CV8U, ddepth = -1 / Mat.CV16S / Mat.CV32F / Mat.CV64F
  \li input.depth = Mat.CV16U / Mat.CV16S, ddepth = -1 / Mat.CV32F / Mat.CV64F
  \li input.depth = Mat.CV32F, ddepth = -1 / Mat.CV32F / Mat.CV64F
  \li input.depth = Mat.64F, ddepth = -1 / Mat.CV64F
  \endlist

  When ddepth = -1, the destination image will have the same depth as the source. In case of 8-bit
  input images, it will result in truncated derivatives
 */

/*!
  \property QSobel::xorder
  \sa Sobel::xorder
 */

/*!
  \qmlproperty int Sobel::xorder

  Order of the derivative in x.
 */

/*!
  \property QSobel::yorder
  \sa Sobel::yorder
 */

/*!
  \qmlproperty int Sobel::yorder

  Order of the derivative in y.
 */

/*!
  \property QSobel::ksize
  \sa Sobel::ksize
 */

/*!
  \qmlproperty int Sobel::ksize

  Size of the extended Sobel kernel; ( must be 1, 3, 5, 7 )
 */

/*!
  \property QSobel::scale
  \sa Sobel::scale
 */

/*!
  \qmlproperty real Sobel::scale

  Optional scale factor for the computed derivative values; by default, no scaling is  applied.
 */

/*!
  \property QSobel::delta
  \sa Sobel::delta
 */

/*!
  \qmlproperty real Sobel::delta

  Optional delta value that is added to the results prior to storing them in output.
 */

/*!
  \property QSobel::borderType
  \sa Sobel::borderType
 */

/*!
  \qmlproperty real Sobel::borderType

  Pixel extrapolation method.
  \sa CopyMakeBorder::BorderType
 */


/*!
  \fn virtual QSGNode* QSobel::updatePaintNode(QSGNode*, QQuickItem::UpdatePaintNodeData*)

  \brief Updates the scene graph node with the drawn lines.

  Parameters :
  \a node
  \a nodeData
 */

QSGNode *QSobel::updatePaintNode(QSGNode *node, QQuickItem::UpdatePaintNodeData *nodeData){
    if ( output()->cvMat()->size() != Size(0, 0) )
        convertScaleAbs(*output()->cvMat(), *m_display->cvMat());

    QMatNode *n = static_cast<QMatNode*>(QMatDisplay::updatePaintNode(node, nodeData));
    static_cast<QSGSimpleMaterial<QMatState>*>(n->material())->state()->mat = m_display;
    return n;
}

/*!
  \brief Filtering function.

  Parameters:
  \a in
  \a out
 */
void QSobel::transform(cv::Mat &in, cv::Mat &out){
    if ( in.size() != Size(0, 0) )
        Sobel(in, out, m_ddepth, m_xorder, m_yorder, m_ksize, m_scale, m_delta, m_borderType);
}
