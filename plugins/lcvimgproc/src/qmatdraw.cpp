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

#include "qmatdraw.h"
#include "opencv2/imgproc.hpp"


/*!
   \class QMatDraw
   \inmodule lcvimgproc_cpp
   \internal
   \brief Draws on a matrix with opencv's drawing functions.
 */

/*!
  \brief QMatDraw contructor

  Parent : \a parent
 */
QMatDraw::QMatDraw(QQuickItem *parent)
    : QMatDisplay(parent)
    , m_in(new QMat(this))
{
}

/*!
  \brief QMatDraw destructor
 */
QMatDraw::~QMatDraw(){
}

void QMatDraw::cleanUp(){
    if ( inputMat() )
        inputMat()->cvMat()->copyTo(*output()->cvMat());
    emit outputChanged();
    update();
}


/*!
  \brief Draws a line from point \a p1 to point \a p2.

  Parameters :
  \a color
  \a thickness
  \a lineType
  \a shift
  \sa MatDraw::line
 */
void QMatDraw::line(
        const QPoint& p1,
        const QPoint& p2,
        const QColor& color,
        int thickness,
        int lineType,
        int shift)
{
    cv::line(
                *output()->cvMat(),
                cv::Point(p1.x(), p1.y()),
                cv::Point(p2.x(), p2.y()),
                cv::Scalar( color.blue(), color.green(), color.red(), color.alpha() ),
                thickness,
                lineType,
                shift
    );
    emit outputChanged();
    update();
}

void QMatDraw::lineOn(
        QMat *mat,
        const QPoint &p1,
        const QPoint &p2,
        const QColor &color,
        int thickness,
        int lineType,
        int shift)
{

    cv::line(
        *mat->cvMat(),
        cv::Point(p1.x(), p1.y()),
        cv::Point(p2.x(), p2.y()),
        cv::Scalar( color.blue(), color.green(), color.red(), color.alpha() ),
        thickness,
        lineType,
        shift
    );
}

/*!
  \brief Draws a rectangle point \a p1 to point \a p2.

  Parameters :
  \a color
  \a thickness
  \a lineType
  \a shift
  \sa MatDraw::rectangle
 */
void QMatDraw::rectangle(
        const QPoint &p1,
        const QPoint &p2,
        const QColor &color,
        int thickness,
        int lineType,
        int shift)
{
    cv::rectangle(
                *output()->cvMat(),
                cv::Point(p1.x(), p1.y()),
                cv::Point(p2.x(), p2.y()),
                cv::Scalar( color.blue(), color.green(), color.red(), color.alpha() ),
                thickness,
                lineType,
                shift
    );
    emit outputChanged();
    update();
}

/*!
  \brief Draws a circle in \a center with \a radius.

  Parameters :
  \a color
  \a thickness
  \a lineType
  \a shift
 */
void QMatDraw::circle(
        const QPoint &center,
        int radius,
        const QColor &color,
        int thickness,
        int lineType,
        int shift)
{
    cv::circle(
                *output()->cvMat(),
                cv::Point(center.x(), center.y()),
                radius,
                cv::Scalar( color.blue(), color.green(), color.red(), color.alpha() ),
                thickness,
                lineType,
                shift
    );
    emit outputChanged();
    update();
}

/*!
  \brief Draws an ellipse

  Parameters :
  \a center
  \a axes
  \a angle
  \a startAngle
  \a endAngle
  \a color
  \a thickness
  \a lineType
  \a shift
 */
void QMatDraw::ellipse(
        const QPoint &center,
        const QSize &axes,
        double angle,
        double startAngle,
        double endAngle,
        const QColor &color,
        int thickness,
        int lineType,
        int shift)
{
    cv::ellipse(
                *output()->cvMat(),
                cv::Point(center.x(), center.y()),
                cv::Size(axes.width(), axes.height()),
                angle,
                startAngle,
                endAngle,
                cv::Scalar( color.blue(), color.green(), color.red(), color.alpha() ),
                thickness,
                lineType,
                shift
    );
    emit outputChanged();
    update();
}

/*!
  \brief Fills an area bounded by one or more polygons.

  Parameters :
  \a points
  \a color
  \a lineType
  \a shift
  \a offset
 */
void QMatDraw::fillPoly(
        const QVariantList &points,
        const QColor &color,
        int lineType,
        int shift,
        const QPoint& offset)
{
    if ( points.size() > 0 ){
        if ( points[0].canConvert<QVariantList>() ){
            cv::Point ** pts = new cv::Point*[points.size()];
            int* npts = new int[points.size()];
            for ( int i = 0; i < points.size(); ++i ){
                QVariantList pointsI = points[i].toList();
                pts[i]  = new cv::Point[pointsI.size()];
                npts[i] = pointsI.size();
                for ( int j = 0; j < pointsI.size(); ++j ){
                    pts[i][j].x = pointsI[j].toPoint().x();
                    pts[i][j].y = pointsI[j].toPoint().y();
                }
            }
            cv::fillPoly(
                        *output()->cvMat(),
                        const_cast<const cv::Point**>(pts),
                        npts,
                        points.size(),
                        cv::Scalar( color.blue(), color.green(), color.red(), color.alpha() ),
                        lineType,
                        shift,
                        cv::Point(offset.x(), offset.y())
            );
            for ( int i = 0; i < points.size(); ++i )
                delete[] pts[i];
            delete[] npts;
            delete[] pts;

        } else {
            cv::Point* pts = new cv::Point[points.size()];
            for ( int i = 0; i < points.size(); ++i ){
                pts[i].x = points[i].toPoint().x();
                pts[i].y = points[i].toPoint().y();
            }
            cv::fillConvexPoly(
                        *output()->cvMat(),
                        pts,
                        points.size(),
                        cv::Scalar( color.blue(), color.green(), color.red(), color.alpha() ),
                        lineType,
                        shift
            );
            delete[] pts;
        }
    }

    emit outputChanged();
    update();
}
