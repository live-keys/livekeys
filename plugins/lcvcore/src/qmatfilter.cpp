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

#include "qmatfilter.h"


/*!
  \qmltype MatFilter
  \instantiates QMatFilter
  \inqmlmodule lcvcore
  \inherits MatDisplay
  \brief Base filter for live cvs filters.

  By inheriting the MatDisplay type, and by adding an input element and a process of transforming it into an output
  element to be displayed on screen is considered a \b{filter type} in live cv.

 */

/*!
  \class QMatFilter
  \inmodule lcvcore_cpp
  \brief  An abstract filter that transforms an input matrix into it's output.

  Besides the QMatDisplay class it inherits, the QMatFilter adds an input element, and a transformation function to
  further ease implementing filters. The transformation function gets called every time the input element is changed.
  This means that to write a simple filter, all you have to do is extend this class and implement the transformation
  function. Here's a small example of a filter that transforms a BGR image into grayscale :

  \code
  class QMatToGrey : public QMatFilter{

      Q_OBJECT

  public:
      QMatToGrey(QQuickItem* parent = 0):QMatFilter(parent){
      }

      void transform(const cv::Mat& in, cv::Mat& out){
          cvtColor(in, out, CV_BGR2GREY);
      }

  }
  \endcode


  With that all that's left is to register the element to QML, after which you can use it like any other Live CV filter :

  \code
  ImRead{
       id : src
       file : project.path + '/sample.jpg'
  }
  MatToGrey{
       input : src.output
  }
  \endcode
 */

/*!
  \brief QMatFilter constructor
  \a parent
 */
QMatFilter::QMatFilter(QQuickItem *parent)
    : QMatDisplay(parent)
    , m_in(QMat::nullMat())
{
}

/*!
  \brief QMatFilter destructor
 */
QMatFilter::~QMatFilter(){
}


/*!
  \property QMatFilter::input
  \sa MatFilter::input
 */

/*!
  \qmlproperty Mat MatFilter::input

  Input matrix to apply the filter to.
 */


/*!
  \fn QMatFilter::transform()
  \brief Transformation function that handles notifications and state changes.

  Call this function in order to trigger the filtering process.
 */
void QMatFilter::transform(){
    if ( isComponentComplete() ){
        try{
            transform(*inputMat()->cvMat(), *output()->cvMat());
            emit outputChanged();
            update();
        } catch (cv::Exception& e ){
            qCritical("%s", e.msg.c_str());
        }
    }
}

/*!
  \brief Function to be implemented by derived classes to apply the filtering process.
  \a in
  \a out
 */
void QMatFilter::transform(const cv::Mat&, cv::Mat&){
}

/*!
  \brief Function used to initiate the transform function once the component is completed.
 */
void QMatFilter::componentComplete(){
    QQuickItem::componentComplete();
    transform();
}
