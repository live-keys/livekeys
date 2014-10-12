#include "QAbsDiff.hpp"

/*!
  \qmltype AbsDiff
  \instantiates QAbsDiff
  \inqmlmodule lcvcore
  \inherits MatFilter
  \brief Performs an absolute difference between two matrixes.

  The example in \b{samples/imgproc/framedifference.qml} shows differentiating two consecutive frames in a video to
  calculate the motion. It uses a MatBuffer to store the previous frame :

  \quotefile imgproc/framedifference.qml
*/

/*!
   \class QAbsDiff
   \inmodule lcvcore_cpp
   \brief This class is used in qml to performs an absolute difference between two matrixes.
 */

/*!
   \brief QAbsDiff constructor
   \a parent
 */
QAbsDiff::QAbsDiff(QQuickItem *parent)
    : QMatFilter(parent)
    , m_in2(new QMat())
    , m_in2Internal(m_in2)
{
}

/*!
   \brief QAbsDiff destructor
 */
QAbsDiff::~QAbsDiff(){
    delete m_in2Internal;
}


/*!
  \property QAbsDiff::input2
  \sa AbsDiff::input2
 */

/*!
  \qmlproperty Mat AbsDiff::input2

  Second input for the subtraction.
 */

/*!
   \brief QAbsDiff transformation function
   \a in
   \a out
 */
void QAbsDiff::transform(cv::Mat &in, cv::Mat &out){
    if ( in.size() == m_in2->cvMat()->size() )
        cv::absdiff(in, *m_in2->cvMat(), out);
}

