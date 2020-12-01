/****************************************************************************
**
** Copyright (C) 2014-2019 Dinu SV.
** (contact: mail@dinusv.com)
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
#include "qhoughlines.h"
#include "opencv2/imgproc.hpp"
#include <vector>

using namespace cv;
using namespace std;

/*!
  \class QHoughLines
  \inmodule lcvimgproc_cpp
  \internal
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

QHoughLines::~QHoughLines(){
    delete d_ptr;
}

/*!
  \brief Filtering function.

  Parameters:
  \a in
  \a out
 */
void QHoughLines::transform(const cv::Mat &in, cv::Mat&){
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
        Mat* surface = output()->internalPtr();
        surface->create(inputMat()->internalPtr()->size(), CV_8UC4);
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
                  m_lineThickness, cv::LINE_AA );
        }
        d->outDirty = false;
    }
    return QMatDisplay::updatePaintNode(node, nodeData);
}
