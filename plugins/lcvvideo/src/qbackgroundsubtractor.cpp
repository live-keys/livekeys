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

#include "qbackgroundsubtractor.h"

using namespace cv;

// QBackgroundSubtractorPrivate Implementation
// -------------------------------------------
QBackgroundSubtractorPrivate::QBackgroundSubtractorPrivate()
    : m_stateId("")
    , m_learningRate(0){
}

QBackgroundSubtractorPrivate::~QBackgroundSubtractorPrivate(){
}

void QBackgroundSubtractorPrivate::deleteSubtractor(){
    qWarning() << "QBackgroundSubtractorPrivate::deleteSubtractor MUST be overridden by a subclass!";
}

cv::BackgroundSubtractor* QBackgroundSubtractorPrivate::subtractor(){
    qWarning() << "QBackgroundSubtractorPrivate::subtractor MUST be overridden by a subclass!";
    return 0;
}

const QString& QBackgroundSubtractorPrivate::stateId() const{
    return m_stateId;
}

void QBackgroundSubtractorPrivate::setStateId(const QString& id){
    m_stateId = id;
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
  \fn void QBackgroundSubtractor::reset()
  \sa BackgroundSubtractor::reset()
 */

/*!
  \qmlmethod void BackgroundSubtractor::reset()

  Resets the state (background model, history, ...) of the subtractor.
 */
void QBackgroundSubtractor::reset(){
    Q_D(QBackgroundSubtractor);
    d->deleteSubtractor();
}

/*!
  \property QBackgroundSubtractor::stateId
  \sa QBackgroundSubtractor::stateId
 */

/*!
  \qmlproperty string QBackgroundSubtractor::stateId

  This property is somehow required. It represents the id of the state container of this filter. The state is used in
  order to store contents between compilations. Give this a unique id in order for the subtractor to not reset between compilations.
 */
const QString& QBackgroundSubtractor::stateId() const{
    Q_D(const QBackgroundSubtractor);
    return d->stateId();
}

void QBackgroundSubtractor::setStateId(const QString& id){
    Q_D(QBackgroundSubtractor);
    if ( d->stateId() != id ){
        d->deleteSubtractor();
        d->setStateId(id);
        emit stateIdChanged();
    }
}

/*!
  \qmlproperty string QBackgroundSubtractor::learningRate

  Learning rate for updating the background model (0 to 1, default is 0).
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
  \fn virtual void QBackgroundSubtractor::transform(cv::Mat& in, cv::Mat& out)
  \brief Filtering function.

  Parameters :
  \a in
  \a out
 */
void QBackgroundSubtractor::transform(Mat& in, Mat& out){
    Q_D(QBackgroundSubtractor);
    BackgroundSubtractor* subtractor = d->subtractor();
    if ( subtractor && !in.empty() )
        subtractor->apply(in, out, d->learningRate());
}
