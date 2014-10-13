#include "QMatResize.hpp"
#include "opencv2/imgproc/imgproc.hpp"

using namespace cv;

/*!
  \qmltype Resize
  \instantiates QMatResize
  \inqmlmodule lcvimgproc
  \inherits MatFilter
  \brief Resizes an image.

  Resizes an image according to the specified filter.

  \quotefile imgproc/resize.qml
*/

/*!
  \class QMatResize
  \inmodule lcvimgproc_cpp
  \brief Finds edges within an image.
 */

/*!
  \brief QMatResize constructor
  Parameters:
  \a parent
 */
QMatResize::QMatResize(QQuickItem *parent)
    : QMatFilter(parent)
    , m_matSize(0, 0)
    , m_fx(0)
    , m_fy(0)
    , m_interpolation(INTER_LINEAR)
{
}

/*!
  \brief QMatResize destructor
 */
QMatResize::~QMatResize(){
}

/*!
  \property QMatResize::matSize
  \sa Resize::matSize
 */

/*!
  \qmlproperty Size Resize::matSize

  Size of the resized matrix. An unspecified size or a size of (0, 0) will make the element look into the
  fx and fy resize factors.
 */

/*!
  \property QMatResize::fx
  \sa Resize::fx
 */

/*!
  \qmlproperty real Resize::fx

  Factor by which to resize on x axis.
 */


/*!
  \property QMatResize::fy
  \sa Resize::fy
 */

/*!
  \qmlproperty real Resize::fy

  Factor by which to resize on y axis.
 */

/*!
  \property QMatResize::interpolation
  \sa Resize::interpolation
 */

/*!
  \qmlproperty Resize::Interpolation Resize::interpolation

  Interpolation method.
 */

/*!
  \brief Filtering function.

  Parameters:
  \a in
  \a out
 */
void QMatResize::transform(cv::Mat& in, cv::Mat& out){
    if ( !in.empty() && (m_matSize != Size(0, 0) || m_fx != 0 || m_fy != 0) ){
        resize(in, out, m_matSize, m_fx, m_fy, m_interpolation);
        if ( out.rows != implicitHeight() )
            setImplicitHeight(out.rows);
        if ( out.cols != implicitWidth() )
            setImplicitWidth(out.cols);
    }
}
