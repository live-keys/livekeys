/****************************************************************************
**
** Copyright (C) 2014-2017 Dinu SV.
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

#include "qbackgroundsubtractor.h"
#include "qstaticcontainer.h"

using namespace cv;


/*!
  \qmltype BackgroundSubtractor
  \instantiates QBackgroundSubtractor
  \inqmlmodule lcvvideo
  \inherits MatFilter
  \brief Background subtractor base type.
 */

// QBackgroundSubtractorPrivate Implementation
// -------------------------------------------

QBackgroundSubtractorPrivate::QBackgroundSubtractorPrivate()
    : m_stateId("")
    , m_learningRate(0){
}

QBackgroundSubtractorPrivate::~QBackgroundSubtractorPrivate(){
}

cv::BackgroundSubtractor* QBackgroundSubtractorPrivate::subtractor(){
    qWarning() << "QBackgroundSubtractorPrivate::subtractor MUST be overridden by a subclass!";
    return 0;
}

double QBackgroundSubtractorPrivate::learningRate() const{
    return m_learningRate;
}

void QBackgroundSubtractorPrivate::setLearningRate(double rate){
    m_learningRate = rate;
}

// QBackgroundSubtractor Implementation
// ------------------------------------

/*!
   \class QBackgroundSubtractor
   \inmodule lcvvideo_cpp

   Base class for background subtractor algorithms.
   Should not be instantiated directly, use a specific background subtractor algorithm instead.
 */

/*!
  \brief QBackgroundSubtractor constructor

  Parameters:
  \a d_ptr Private pointer of a subclass instance
  \a parent
 */
QBackgroundSubtractor::QBackgroundSubtractor(QBackgroundSubtractorPrivate *d_ptr, QQuickItem *parent)
    : QMatFilter(parent)
    , d_ptr(d_ptr ? d_ptr : new QBackgroundSubtractorPrivate){
    if ( !d_ptr ){
        qWarning() << "QBackgroundSubtractor may not be initialized directly!"
                   << "Instantiate a subclass instead.";
    }
}

/*!
  \brief QBackgroundSubtractor destructor
 */
QBackgroundSubtractor::~QBackgroundSubtractor(){
}


/*!
  \qmlproperty string BackgroundSubtractor::learningRate

  Learning rate for updating the background model (0 to 1, default is 0).
 */

/*!
  \property QBackgroundSubtractor::learningRate
  \sa BackgroundSubtractor::learningRate
 */

double QBackgroundSubtractor::learningRate() const{
    Q_D(const QBackgroundSubtractor);
    return d->learningRate();
}

void QBackgroundSubtractor::setLearningRate(double rate){
    Q_D(QBackgroundSubtractor);
    if ( d->learningRate() != rate ){
        d->setLearningRate(rate);
        emit learningRateChanged();
    }
}

/*!
  \fn virtual void QBackgroundSubtractor::transform(const cv::Mat& in, cv::Mat& out)
  \brief Filtering function.

  Parameters :
  \a in
  \a out
 */
void QBackgroundSubtractor::transform(const Mat& in, Mat& out){
    Q_D(QBackgroundSubtractor);
    BackgroundSubtractor* subtractor = d->subtractor();
    if ( subtractor && !in.empty() )
        subtractor->apply(in, out, d->learningRate());
}
