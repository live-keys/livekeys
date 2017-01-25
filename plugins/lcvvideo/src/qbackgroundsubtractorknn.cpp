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

#include "qbackgroundsubtractorknn.h"
#include "qstatecontainer.h"

using namespace cv;

/*!
  \qmltype BackgroundSubtractorKnn
  \instantiates QBackgroundSubtractorKnn
  \inqmlmodule lcvvideo
  \inherits BackgroundSubtractor
  \brief K-nearest neigbours based background/foreground segmentation algorithm

  K-nearest neigbours based background/foreground segmentation algorithm.
*/

/*!
   \class QBackgroundSubtractorKnn
   \inmodule lcvvideo_cpp
   \brief K-nearest neigbours based background/foreground segmentation algorithm
 */

class QBackgroundSubtractorKnnPrivate : public QBackgroundSubtractorPrivate{

public:
    QBackgroundSubtractorKnnPrivate();
    ~QBackgroundSubtractorKnnPrivate();

    virtual void deleteSubtractor();
    virtual BackgroundSubtractor* subtractor();

    BackgroundSubtractorKNN* subtractorKnn();

public:
    bool detectShadows;
    double dist2Threshold;
    int history;
    int knnSamples;
    int nSamples;
    double shadowThreshold;
    int shadowValue;

private:
    Ptr<BackgroundSubtractorKNN>* createSubtractor();

private:
    Ptr<BackgroundSubtractorKNN>* m_subtractorKnn;

};

// QBackgroundSubtractorKnnPrivate Implementation
// ----------------------------------------------
QBackgroundSubtractorKnnPrivate::QBackgroundSubtractorKnnPrivate() :
    detectShadows(true),
    dist2Threshold(400.0),
    history(500),
    knnSamples(3),
    nSamples(7),
    shadowThreshold(0.5),
    shadowValue(127),
    m_subtractorKnn(0){
}

QBackgroundSubtractorKnnPrivate::~QBackgroundSubtractorKnnPrivate(){
    if ( stateId() == "" ) // otherwise leave it to the QStateContiner
        delete m_subtractorKnn;
}

Ptr<BackgroundSubtractorKNN>* QBackgroundSubtractorKnnPrivate::createSubtractor(){
    Ptr<BackgroundSubtractorKNN> subtractor = createBackgroundSubtractorKNN(history, dist2Threshold, detectShadows);
    subtractor->setNSamples(nSamples);
    subtractor->setkNNSamples(knnSamples);
    subtractor->setShadowValue(shadowValue);
    subtractor->setShadowThreshold(shadowThreshold);
    return new Ptr<BackgroundSubtractorKNN>(subtractor);
}

void QBackgroundSubtractorKnnPrivate::deleteSubtractor(){
    QStateContainer<Ptr<BackgroundSubtractorKNN>>& stateCont =
            QStateContainer<Ptr<BackgroundSubtractorKNN>>::instance();
    stateCont.registerState(stateId(), static_cast<Ptr<BackgroundSubtractorKNN>*>(0));
    delete m_subtractorKnn;
    m_subtractorKnn = 0;
}

BackgroundSubtractorKNN* QBackgroundSubtractorKnnPrivate::subtractorKnn(){
    if ( !m_subtractorKnn ){
        if ( stateId() != "" ){
            QStateContainer<Ptr<BackgroundSubtractorKNN>>& stateCont =
                    QStateContainer<Ptr<BackgroundSubtractorKNN>>::instance();

            m_subtractorKnn = stateCont.state(stateId());
            if ( m_subtractorKnn == 0 ){
                m_subtractorKnn = createSubtractor();
                stateCont.registerState(stateId(), m_subtractorKnn);
            }
        } else {
            qWarning("QBackgroundSubtractorKnn does not have a stateId assigned and cannot save "
                     "it\'s state over multiple compilations. Set a unique stateId to "
                     "avoid this problem.");
            m_subtractorKnn = createSubtractor();
        }
    }
    return m_subtractorKnn->get();
}

BackgroundSubtractor* QBackgroundSubtractorKnnPrivate::subtractor(){
    return subtractorKnn();
}

// QBackgroundSubtractorKnn Implementation
// ---------------------------------------

/*!
  \brief QBackgroundSubtractorKnn constructor

  Parameters:
  \a parent
 */
QBackgroundSubtractorKnn::QBackgroundSubtractorKnn(QQuickItem *parent) :
    QBackgroundSubtractor(new QBackgroundSubtractorKnnPrivate, parent),
    d_ptr(static_cast<QBackgroundSubtractorKnnPrivate*>(QBackgroundSubtractor::d_ptr)){
}

/*!
  \brief QBackgroundSubtractorKnn destructor
 */
QBackgroundSubtractorKnn::~QBackgroundSubtractorKnn(){
}

/*!
  \property QBackgroundSubtractorKnn::detectShadows
  \sa BackgroundSubtractorKnn::detectShadows
 */

/*!
  \qmlproperty bool BackgroundSubtractorKnn::detectShadows

  Enables or disables shadow detection.
 */
bool QBackgroundSubtractorKnn::detectShadows() const{
    Q_D(const QBackgroundSubtractorKnn);
    return d->detectShadows;
}

void QBackgroundSubtractorKnn::setDetectShadows(bool detectShadows){
    Q_D(QBackgroundSubtractorKnn);
    if ( d->detectShadows != detectShadows ){
        d->detectShadows = detectShadows;
        if ( d->subtractorKnn() )
            d->subtractorKnn()->setDetectShadows(detectShadows);
        emit detectShadowsChanged();
    }
}

/*!
  \property QBackgroundSubtractorKnn::dist2Threshold
  \sa BackgroundSubtractorKnn::dist2Threshold
 */

/*!
  \qmlproperty double BackgroundSubtractorKnn::dist2Threshold

  Threshold on the squared distance.
 */
double QBackgroundSubtractorKnn::dist2Threshold() const{
    Q_D(const QBackgroundSubtractorKnn);
    return d->dist2Threshold;
}

void QBackgroundSubtractorKnn::setDist2Threshold(double dist2Threshold){
    Q_D(QBackgroundSubtractorKnn);
    if ( d->dist2Threshold != dist2Threshold ){
        d->dist2Threshold = dist2Threshold;
        if ( d->subtractorKnn() )
            d->subtractorKnn()->setDist2Threshold(dist2Threshold);
        emit dist2ThresholdChanged();
    }
}

/*!
  \property QBackgroundSubtractorKnn::history
  \sa BackgroundSubtractorKnn::history
 */

/*!
  \qmlproperty int BackgroundSubtractorKnn::history

  Number of last frames that affect the background model.
 */
int QBackgroundSubtractorKnn::history() const{
    Q_D(const QBackgroundSubtractorKnn);
    return d->history;
}

void QBackgroundSubtractorKnn::setHistory(int history){
    Q_D(QBackgroundSubtractorKnn);
    if ( d->history != history ){
        d->history = history;
        if ( d->subtractorKnn() )
            d->subtractorKnn()->setHistory(history);
        emit historyChanged();
    }
}

/*!
  \property QBackgroundSubtractorKnn::knnSamples
  \sa BackgroundSubtractorKnn::knnSamples
 */

/*!
  \qmlproperty int BackgroundSubtractorKnn::knnSamples

  How many nearest neigbours need to match.
 */
int QBackgroundSubtractorKnn::knnSamples() const{
    Q_D(const QBackgroundSubtractorKnn);
    return d->knnSamples;
}

void QBackgroundSubtractorKnn::setKnnSamples(int knnSamples){
    Q_D(QBackgroundSubtractorKnn);
    if ( d->knnSamples != knnSamples ){
        d->knnSamples = knnSamples;
        if ( d->subtractorKnn() )
            d->subtractorKnn()->setkNNSamples(knnSamples);
        emit knnSamplesChanged();
    }
}

/*!
  \property QBackgroundSubtractorKnn::nSamples
  \sa BackgroundSubtractorKnn::nSamples
 */

/*!
  \qmlproperty int BackgroundSubtractorKnn::nSamples

  Number of data samples in the background model.
 */
int QBackgroundSubtractorKnn::nSamples() const{
    Q_D(const QBackgroundSubtractorKnn);
    return d->nSamples;
}

void QBackgroundSubtractorKnn::setNSamples(int nSamples){
    Q_D(QBackgroundSubtractorKnn);
    if ( d->nSamples != nSamples ){
        d->nSamples = nSamples;
        if ( d->subtractorKnn() )
            d->subtractorKnn()->setNSamples(nSamples);
        emit nSamplesChanged();
    }
}

/*!
  \property QBackgroundSubtractorKnn::shadowThreshold
  \sa BackgroundSubtractorKnn::shadowThreshold
 */

/*!
  \qmlproperty double BackgroundSubtractorKnn::shadowThreshold

  Shadow threshold.
 */
double QBackgroundSubtractorKnn::shadowThreshold() const{
    Q_D(const QBackgroundSubtractorKnn);
    return d->shadowThreshold;
}

void QBackgroundSubtractorKnn::setShadowThreshold(double shadowThreshold){
    Q_D(QBackgroundSubtractorKnn);
    if ( d->shadowThreshold != shadowThreshold ){
        d->shadowThreshold = shadowThreshold;
        if ( d->subtractorKnn() )
            d->subtractorKnn()->setShadowThreshold(shadowThreshold);
        emit shadowThresholdChanged();
    }
}

/*!
  \property QBackgroundSubtractorKnn::shadowValue
  \sa BackgroundSubtractorKnn::shadowValue
 */

/*!
  \qmlproperty int BackgroundSubtractorKnn::shadowValue

  Pixel value for pixels detected as shadow.
 */
int QBackgroundSubtractorKnn::shadowValue() const{
    Q_D(const QBackgroundSubtractorKnn);
    return d->shadowValue;
}

void QBackgroundSubtractorKnn::setShadowValue(int shadowValue){
    Q_D(QBackgroundSubtractorKnn);
    if ( d->shadowValue != shadowValue ){
        d->shadowValue = shadowValue;
        if ( d->subtractorKnn() )
            d->subtractorKnn()->setShadowValue(shadowValue);
        emit shadowValueChanged();
    }
}
