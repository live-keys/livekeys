/****************************************************************************
**
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

#include "qbilateralfilter.h"

using namespace cv;

/*!
  \qmltype BilateralFilter
  \instantiates QBilateralFilter
  \inqmlmodule lcvimgproc
  \inherits MatFilter
  \brief Applies the bilateral filter to an image.

  \quotefile imgproc/bilateralfilter.qml
*/

/*!
   \class QBilateralFilter
   \inmodule lcvimgproc_cpp
   \brief Applies the bilateral filter to an image.
 */

class QBilateralFilterPrivate{

public:
    QBilateralFilterPrivate();
    ~QBilateralFilterPrivate();

public:
    int dVal;
    double sigmaColor;
    double sigmaSpace;
    int borderType;

};

// QBilateralFilterPrivate Implementation
// --------------------------------------
QBilateralFilterPrivate::QBilateralFilterPrivate() :
    dVal(9),
    sigmaColor(75),
    sigmaSpace(75),
    borderType(BORDER_DEFAULT){
}

QBilateralFilterPrivate::~QBilateralFilterPrivate(){
}

// QBilateralFilter Implementation
// -------------------------------

/*!
  \brief QBilateralFilter constructor

  Parameters:
  \a parent
 */
QBilateralFilter::QBilateralFilter(QQuickItem *parent) :
    QMatFilter(parent),
    d_ptr(new QBilateralFilterPrivate){
}

/*!
  \brief QBilateralFilter destructor
 */
QBilateralFilter::~QBilateralFilter(){
}

/*!
  \property QBilateralFilter::d
  \sa BilateralFilter::d
 */

/*!
  \qmlproperty int BilateralFilter::d

  Diameter of each pixel neighborhood that is used during filtering.
 */
int QBilateralFilter::d() const{
    Q_D(const QBilateralFilter);
    return d->dVal;
}

void QBilateralFilter::setD(int dVal){
    Q_D(QBilateralFilter);
    if ( d->dVal != dVal ){
        d->dVal = dVal;
        emit dChanged();
    }
}

/*!
  \property QBilateralFilter::sigmaColor
  \sa BilateralFilter::sigmaColor
 */

/*!
  \qmlproperty double BilateralFilter::sigmaColor

  Filter sigma in the color space.
 */
double QBilateralFilter::sigmaColor() const{
    Q_D(const QBilateralFilter);
    return d->sigmaColor;
}

void QBilateralFilter::setSigmaColor(double sigmaColor){
    Q_D(QBilateralFilter);
    if ( d->sigmaColor != sigmaColor ){
        d->sigmaColor = sigmaColor;
        emit sigmaColorChanged();
    }
}

/*!
  \property QBilateralFilter::sigmaSpace
  \sa BilateralFilter::sigmaSpace
 */

/*!
  \qmlproperty double BilateralFilter::sigmaSpace

  Filter sigma in the coordinate space.
 */
double QBilateralFilter::sigmaSpace() const{
    Q_D(const QBilateralFilter);
    return d->sigmaSpace;
}

void QBilateralFilter::setSigmaSpace(double sigmaSpace){
    Q_D(QBilateralFilter);
    if ( d->sigmaSpace != sigmaSpace ){
        d->sigmaSpace = sigmaSpace;
        emit sigmaSpaceChanged();
    }
}

/*!
  \property QBilateralFilter::borderType
  \sa BilateralFilter::borderType
 */

/*!
  \qmlproperty int BilateralFilter::borderType

  Pixel extrapolation method.
 */
int QBilateralFilter::borderType() const{
    Q_D(const QBilateralFilter);
    return d->borderType;
}

void QBilateralFilter::setBorderType(int borderType){
    Q_D(QBilateralFilter);
    if ( d->borderType != borderType ){
        d->borderType = borderType;
        emit borderTypeChanged();
    }
}

/*!
  \brief Filter processing function.

  Params:
  \a in
  \a out
 */
void QBilateralFilter::transform(Mat& in, Mat& out){
    Q_D(const QBilateralFilter);
    bilateralFilter(in, out, d->dVal, d->sigmaColor, d->sigmaSpace, d->borderType);
}
