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
#include "qhoughlines.h"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
#include <vector>

using namespace cv;
using namespace std;

/*!
  \qmltype HoughLines
  \instantiates QHoughLines
  \inqmlmodule lcvimgproc
  \inherits MatFilter
  \brief Finds line segments in a binary image using the Hough transform.

  \quotefile imgproc/houghlines.qml
*/

/*!
  \class QHoughLines
  \inmodule lcvimgproc_cpp
  \brief Finds line segments in a binary image using the Hough transform.
 */

class QHoughLinesPrivate{

public:
    vector<Vec2f> lines;
    bool          linesDirty;
    bool          outDirty;

};

/*!
  \brief QHoughLines constructor

  Parameters:
  \a parent
 */
QHoughLines::QHoughLines(QQuickItem *parent)
    : QMatFilter(parent)
    , m_srn(0)
    , m_stn(0)
    , m_lineColor(0, 255, 0)
    , m_lineThickness(2)
    , d_ptr(new QHoughLinesPrivate){

    Q_D(QHoughLines);
    d->linesDirty = false;
    d->outDirty   = false;
}

/*!
  \brief QHoughLines destructor
 */
QHoughLines::~QHoughLines(){
    delete d_ptr;
}

/*!
  \property QHoughLines::rho
  \sa HoughLines::rho
 */

/*!
  \qmlproperty real HoughLines::rho

  Distance resolution of the accumulator in pixels.
 */

/*!
  \property QHoughLines::theta
  \sa HoughLines::theta
 */

/*!
  \qmlproperty real HoughLines::theta

  Angle resolution of the accumulator in radians.
 */

/*!
  \property QHoughLines::threshold
  \sa HoughLines::threshold
 */

/*!
  \qmlproperty int HoughLines::threshold

  Accumulator threshold parameter. Only those lines are returned that get enough votes ( > threshold ).
 */

/*!
  \property QHoughLines::srn
  \sa HoughLines::srn
 */

/*!
  \qmlproperty real HoughLines::srn

  For the multi-scale Hough transform, it is a divisor for the distance resolution rho . The coarse accumulator distance
  resolution is rho and the accurate accumulator resolution is rho/srn . If both srn=0 and stn=0 , the classical Hough
  transform is used. Otherwise, both these parameters should be positive.
 */

/*!
  \property QHoughLines::stn
  \sa HoughLines::stn
 */

/*!
  \qmlproperty real HoughLines::stn

  For the multi-scale Hough transform, it is a divisor for the distance resolution theta.
 */

/*!
  \property QHoughLines::lineColor
  \sa HoughLines::lineColor
 */

/*!
  \qmlproperty color HoughLines::lineColor

  Color of the lines being drawn.
 */

/*!
  \property QHoughLines::lineThickness
  \sa HoughLines::lineColor
 */

/*!
  \qmlproperty int HoughLines::lineThickness

  Thickness of the lines being drawn.
 */


/*!
  \brief Filtering function.

  Parameters:
  \a in
  \a out
 */
void QHoughLines::transform(cv::Mat &in, cv::Mat&){
    Q_D(QHoughLines);
    if ( in.size() != Size(0, 0) )
        HoughLines(in, d->lines, m_rho, m_theta, m_threshold, m_srn, m_stn);
    d->linesDirty = true;
    d->outDirty   = true;
}

/*!
  \fn virtual QSGNode* QHoughLines::updatePaintNode(QSGNode*, UpdatePaintNodeData*)

  \brief Updates the scene graph node with the drawn lines.

  Parameters :
  \a node
  \a nodeData
 */

QSGNode *QHoughLines::updatePaintNode(QSGNode *node, QQuickItem::UpdatePaintNodeData *nodeData){
    Q_D(QHoughLines);
    if ( d->outDirty ){
        Mat* surface = output()->cvMat();
        surface->create(inputMat()->cvMat()->size(), CV_8UC4);
        surface->setTo(Scalar(0, 0, 0, 0));
        for( size_t i = 0; i < d->lines.size(); ++i ){
            float rho = d->lines[i][0], theta = d->lines[i][1];
            double a  = cos(theta), b = sin(theta);
            double x0 = a * rho, y0 = b * rho;
            Point pt1, pt2;
            pt1.x = cvRound(x0 + 1000*(-b));
            pt1.y = cvRound(y0 + 1000*(a));
            pt2.x = cvRound(x0 - 1000*(-b));
            pt2.y = cvRound(y0 - 1000*(a));
            line( *surface, pt1, pt2,
                  Scalar(m_lineColor.blue(), m_lineColor.green(), m_lineColor.red(), 255),
                  m_lineThickness, CV_AA );
        }
        d->outDirty = false;
    }
    return QMatDisplay::updatePaintNode(node, nodeData);
}
