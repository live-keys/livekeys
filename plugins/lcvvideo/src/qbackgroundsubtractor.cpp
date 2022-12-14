/****************************************************************************
**
** Copyright (C) 2022 Dinu SV.
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

cv::BackgroundSubtractor* QBackgroundSubtractorPrivate::subtractor(){
    qWarning() << "QBackgroundSubtractorPrivate::subtractor is an abstract type!";
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

/**
 * \class QBackgroundSubtractor
 * \ingroup plugin-lcvvideo
 * 
 * \brief Base class for background subtractor algorithms.
 *  
 * Should not be instantiated directly, use a specific background subtractor algorithm instead.
 */

/**
* \brief QBackgroundSubtractor constructor
* Parameters:
* 
* \a d_ptr Private pointer of a subclass instance
* 
* \a parent
*/
QBackgroundSubtractor::QBackgroundSubtractor(QBackgroundSubtractorPrivate *d_ptr, QQuickItem *parent)
    : QObject(parent)
    , d_ptr(d_ptr ? d_ptr : new QBackgroundSubtractorPrivate){
    if ( !d_ptr ){
        qWarning() << "QBackgroundSubtractor may not be initialized directly!"
                   << "Instantiate a subclass instead.";
    }
}

/**
 * \brief QBackgroundSubtractor destructor
 */
QBackgroundSubtractor::~QBackgroundSubtractor(){
}


/**
 * \property QBackgroundSubtractor::learningRate
 * \sa BackgroundSubtractor::learningRate
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

/**
 * \fn virtual void QBackgroundSubtractor::transform(const cv::Mat& in, cv::Mat& out)
 * \brief Filtering function.
 * 
 * Parameters :
 * 
 * \a in
 * 
 * \a out
 */
void QBackgroundSubtractor::transform(const Mat& in, Mat& out){
    Q_D(QBackgroundSubtractor);
    BackgroundSubtractor* subtractor = d->subtractor();
    if ( subtractor && !in.empty() )
        subtractor->apply(in, out, d->learningRate());
}

QMat *QBackgroundSubtractor::subtract(QMat *input){
    QMat* out = new QMat;
    transform(input->internal(), out->internal());
    return out;
}
