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
  \qmltype MatDraw
  \instantiates QMatDraw
  \inqmlmodule lcvcore
  \inherits MatDisplay
  \brief Provides matrix drawing operations.

  Provides the basic set of functions needed in order to draw on a matrix.

  \quotefile imgproc/drawing.qml
*/

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


/*!
  \qmlproperty Mat MatDraw::input

  Input surface to draw on.
 */


/*!
  \qmlmethod MatDraw::cleanUp()

  Cleans up the matrix surface to it's initial value.
 */

void QMatDraw::cleanUp(){
    if ( inputMat() )
        inputMat()->cvMat()->copyTo(*output()->cvMat());
    emit outputChanged();
    update();
}

/*!
  \qmlmethod MatDraw::line(Point p1, Point p2, Color color, int thickness, int lineType, int shift)

  Draw a line from point \a p1 to point \a p2 of a specified \a color and \a thickness. The \a lineType can be
  4 ( 4-connected line ), 8 (8-connected line, also the default) and -1 (antialiased line). The shift is the number
  of fractional bits in the point coordinates.
 */

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
  \qmlmethod MatDraw::rectangle(Point p1, Point p2, Color color, int thickness, int lineType, int shift)

  Draw a rectangle from point \a p1 to point \a p2 of a specified \a color and \a thickness. The \a lineType can be
  4 ( 4-connected line ), 8 (8-connected line, also the default) and -1 (antialiased line). The shift is the number
  of fractional bits in the point coordinates.
 */

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
  \qmlmethod MatDraw::circle(Point center, int radius, Color color, int thickness, int lineType, int shift)

  Draw a circle by specifying its \a center point, \a radius, \a color and \a thickness. For the lineType and shift, see
  the lin method.

  \sa MatDraw::line
 */

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
  \qmlmethod MatDraw::ellipse(Point center, Size axes, real angle, real startAngle, real endAngle, Color color,
  int thickness, int lineType, int shift)

  Draw an ellipse in the \a center point, with its \a axes (half of the size of the ellipse main axes), rotation
  \a angle in degrees, \a startAngle and \a endAngle of the elliptic arc in degrees and \a color. For the lineType and
  shift see the line method.

  \sa MatDraw::line
 */

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
  \qmlmethod MatDraw::fillPoly(variant points, Color color, int lineType, int shift, Point offset)

  Fills the area bounded by one or more polygons. The \a points is the array of polygons where each polygon is
  represented by an array of points, the \a color is the polygons color, the \a lineType is the type of the polygon
  boundaries and the shift is the number of fractional bits in the vertex coordinates. The offset is an optional offset
  of all points of the contours.

  \sa MatDraw::line
 */

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
