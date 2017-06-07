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

#include "qbackgroundsubtractormog2.h"
#include "qstaticcontainer.h"

using namespace cv;

/*!
  \qmltype BackgroundSubtractorMog2
  \instantiates QBackgroundSubtractorMog2
  \inqmlmodule lcvvideo
  \inherits BackgroundSubtractor
  \brief Gaussian mixture based background/foreground segmentation algorithm.  This is a \b {static item}.

  \quotefile video/backgroundsubtractormog2.qml
*/

/*!
   \class QBackgroundSubtractorMog2
   \inmodule lcvvideo_cpp
   \internal
   \brief Gaussian mixture based background/foreground segmentation algorithm.
 */

class QBackgroundSubtractorMog2Private : public QBackgroundSubtractorPrivate{

public:
    QBackgroundSubtractorMog2Private();
    virtual ~QBackgroundSubtractorMog2Private();

    virtual BackgroundSubtractor* subtractor();

    BackgroundSubtractorMOG2* subtractorMog2();

public:
    QMat backgroundModel;
    int history;
    int nmixtures;
    uchar nShadowDetection;
    bool detectShadows;
    float backgroundRatio;
    float ct;
    float tau;
    float varInit;
    float varMin;
    float varMax;
    float varThreshold;
    float varThresholdGen;

    Ptr<BackgroundSubtractorMOG2>* createSubtractor();

    Ptr<BackgroundSubtractorMOG2>* m_subtractorMog2;

};

// QBackgroundSubtractorMog2Private Implementation
// -------------------------------------------
QBackgroundSubtractorMog2Private::QBackgroundSubtractorMog2Private()
    : QBackgroundSubtractorPrivate()
    , history(500)
    , nmixtures(5)
    , nShadowDetection(127)
    , detectShadows(true)
    , backgroundRatio(0.9f)
    , ct(0.05f)
    , tau(0.5f)
    , varInit(15)
    , varMin(4)
    , varMax(75)
    , varThreshold(16)
    , varThresholdGen(9)
    , m_subtractorMog2(0){
}

QBackgroundSubtractorMog2Private::~QBackgroundSubtractorMog2Private(){
}

Ptr<BackgroundSubtractorMOG2>* QBackgroundSubtractorMog2Private::createSubtractor(){
    Ptr<BackgroundSubtractorMOG2> subtractor = createBackgroundSubtractorMOG2(history, varThreshold, detectShadows);
    subtractor->setNMixtures(nmixtures);
    subtractor->setShadowValue(nShadowDetection);
    subtractor->setBackgroundRatio(backgroundRatio);
    subtractor->setComplexityReductionThreshold(ct);
    subtractor->setShadowThreshold(tau);
    subtractor->setVarInit(varInit);
    subtractor->setVarMin(varMin);
    subtractor->setVarMax(varMax);
    subtractor->setVarThresholdGen(varThresholdGen);
    return new Ptr<BackgroundSubtractorMOG2>(subtractor);
}

BackgroundSubtractorMOG2* QBackgroundSubtractorMog2Private::subtractorMog2(){
    if ( !m_subtractorMog2 )
        return 0;
    return m_subtractorMog2->get();
}

BackgroundSubtractor* QBackgroundSubtractorMog2Private::subtractor(){
    return subtractorMog2();
}

// QBackgroundSubtractorMog2 Implementation
// ------------------------------------

/*!
  \brief QBackgroundSubtractorMog2 constructor

  Parameters:
  \a parent
 */
QBackgroundSubtractorMog2::QBackgroundSubtractorMog2(QQuickItem *parent)
    : QBackgroundSubtractor(new QBackgroundSubtractorMog2Private, parent)
    , d_ptr(static_cast<QBackgroundSubtractorMog2Private*>(QBackgroundSubtractor::d_ptr)){
}

/*!
  \brief QBackgroundSubtractorMog2 destructor
 */
QBackgroundSubtractorMog2::~QBackgroundSubtractorMog2(){
}

/*!
  \property QBackgroundSubtractorMog2::backgroundModel
  \sa BackgroundSubtractorMog2::backgroundModel
 */

/*!
  \qmlproperty Mat BackgroundSubtractorMog2::backgroundModel

  Snapshot of the background model computed by the MOG2 algorithm.
 */
QMat* QBackgroundSubtractorMog2::backgroundModel(){
    Q_D(QBackgroundSubtractorMog2);
    if ( d->subtractorMog2() )
        d->subtractorMog2()->getBackgroundImage(*(d->backgroundModel.cvMat()));
    return &d->backgroundModel;
}

/*!
  \property QBackgroundSubtractorMog2::history
  \sa BackgroundSubtractorMog2::history
 */

/*!
  \qmlproperty int BackgroundSubtractorMog2::history

  Length of the history. Defaults to 500.
 */
int QBackgroundSubtractorMog2::history() const{
    Q_D(const QBackgroundSubtractorMog2);
    return d->history;
}

void QBackgroundSubtractorMog2::setHistory(int history){
    Q_D(QBackgroundSubtractorMog2);
    if ( d->history != history ){
         d->history = history;
         if ( d->subtractorMog2() )
            d->subtractorMog2()->setHistory(history);
         emit historyChanged();
    }
}

/*!
  \property QBackgroundSubtractorMog2::nmixtures
  \sa BackgroundSubtractorMog2::nmixtures
 */

/*!
  \qmlproperty int BackgroundSubtractorMog2::nmixtures

  Maximum allowed number of mixture components. Defaults to 5.
 */
int QBackgroundSubtractorMog2::nmixtures() const{
    Q_D(const QBackgroundSubtractorMog2);
    return d->nmixtures;
}

void QBackgroundSubtractorMog2::setNmixtures(int nmixtures){
    Q_D(QBackgroundSubtractorMog2);
    if ( d->nmixtures != nmixtures ){
         d->nmixtures = nmixtures;
         if ( d->subtractorMog2() )
            d->subtractorMog2()->setNMixtures(nmixtures);
         emit nmixturesChanged();
    }
}

/*!
  \property QBackgroundSubtractorMog2::nShadowDetection
  \sa BackgroundSubtractorMog2::nShadowDetection
 */

/*!
  \qmlproperty int BackgroundSubtractorMog2::nShadowDetection

  The value for marking shadow pixels in the output foreground mask.
  Must be in the range 0-255. Defaults to 127.
 */
int QBackgroundSubtractorMog2::nShadowDetection() const{
    Q_D(const QBackgroundSubtractorMog2);
    return d->nShadowDetection;
}

void QBackgroundSubtractorMog2::setNShadowDetection(int nShadowDetection){
    Q_D(QBackgroundSubtractorMog2);
    if ( nShadowDetection > 255 )
        nShadowDetection = 255;
    if ( nShadowDetection < 0 )
        nShadowDetection = 0;
    if ( d->nShadowDetection != nShadowDetection ){
         d->nShadowDetection = nShadowDetection;
         if ( d->subtractorMog2() )
            d->subtractorMog2()->setShadowValue(nShadowDetection);
         emit nShadowDetectionChanged();
    }
}

/*!
  \property QBackgroundSubtractorMog2::detectShadows
  \sa BackgroundSubtractorMog2::detectShadows
 */

/*!
  \qmlproperty bool BackgroundSubtractorMog2::detectShadows

  Whether shadow detection should be enabled. Defaults to false.
 */
bool QBackgroundSubtractorMog2::detectShadows() const{
    Q_D(const QBackgroundSubtractorMog2);
    return d->detectShadows;
}

void QBackgroundSubtractorMog2::setDetectShadows(bool detectShadows){
    Q_D(QBackgroundSubtractorMog2);
    if ( d->detectShadows != detectShadows ){
         d->detectShadows = detectShadows;
         if ( d->subtractorMog2() )
            d->subtractorMog2()->setDetectShadows(detectShadows);
         emit detectShadowsChanged();
    }
}

/*!
  \property QBackgroundSubtractorMog2::backgroundRatio
  \sa BackgroundSubtractorMog2::backgroundRatio
 */

/*!
  \qmlproperty double BackgroundSubtractorMog2::backgroundRatio

  Threshold defining whether the component is significant enough to be included into the background model.
  Defaults to 0.9.
 */
float QBackgroundSubtractorMog2::backgroundRatio() const{
    Q_D(const QBackgroundSubtractorMog2);
    return d->backgroundRatio;
}

void QBackgroundSubtractorMog2::setBackgroundRatio(float backgroundRatio){
    Q_D(QBackgroundSubtractorMog2);
    if ( d->backgroundRatio != backgroundRatio ){
         d->backgroundRatio = backgroundRatio;
         if ( d->subtractorMog2() )
            d->subtractorMog2()->setBackgroundRatio(backgroundRatio);
         emit backgroundRatioChanged();
    }
}

/*!
  \property QBackgroundSubtractorMog2::ct
  \sa BackgroundSubtractorMog2::ct
 */

/*!
  \qmlproperty double BackgroundSubtractorMog2::ct

  Complexity reduction parameter. Defaults to 0.05.
 */
float QBackgroundSubtractorMog2::ct() const{
    Q_D(const QBackgroundSubtractorMog2);
    return d->ct;
}

void QBackgroundSubtractorMog2::setCt(float ct){
    Q_D(QBackgroundSubtractorMog2);
    if ( d->ct != ct ){
         d->ct = ct;
         if ( d->subtractorMog2() )
            d->subtractorMog2()->setComplexityReductionThreshold(ct);
         emit ctChanged();
    }
}

/*!
  \qmlproperty double BackgroundSubtractorMog2::tau

  Shadow threshold. Defaults to 0.5.
 */
float QBackgroundSubtractorMog2::tau() const{
    Q_D(const QBackgroundSubtractorMog2);
    return d->tau;
}

void QBackgroundSubtractorMog2::setTau(float tau){
    Q_D(QBackgroundSubtractorMog2);
    if ( d->tau != tau ){
         d->tau = tau;
         if ( d->subtractorMog2() )
            d->subtractorMog2()->setShadowThreshold(tau);
         emit tauChanged();
    }
}

/*!
  \qmlproperty double BackgroundSubtractorMog2::varInit

  Initial variance for the newly generated components. Defaults to 15.
 */
float QBackgroundSubtractorMog2::varInit() const{
    Q_D(const QBackgroundSubtractorMog2);
    return d->varInit;
}

void QBackgroundSubtractorMog2::setVarInit(float varInit){
    Q_D(QBackgroundSubtractorMog2);
    if ( d->varInit != varInit ){
         d->varInit = varInit;
         if ( d->subtractorMog2() )
            d->subtractorMog2()->setVarInit(varInit);
         emit varInitChanged();
    }
}

/*!
  \qmlproperty double BackgroundSubtractorMog2::varMin

  Parameter used to further control the variance. Defaults to 4.
 */
float QBackgroundSubtractorMog2::varMin() const{
    Q_D(const QBackgroundSubtractorMog2);
    return d->varMin;
}

void QBackgroundSubtractorMog2::setVarMin(float varMin){
    Q_D(QBackgroundSubtractorMog2);
    if ( d->varMin != varMin ){
         d->varMin = varMin;
         if ( d->subtractorMog2() )
            d->subtractorMog2()->setVarMin(varMin);
         emit varMinChanged();
    }
}

/*!
  \qmlproperty double BackgroundSubtractorMog2::varMax

  Parameter used to further control the variance. Defaults to 75.
 */
float QBackgroundSubtractorMog2::varMax() const{
    Q_D(const QBackgroundSubtractorMog2);
    return d->varMax;
}

void QBackgroundSubtractorMog2::setVarMax(float varMax){
    Q_D(QBackgroundSubtractorMog2);
    if ( d->varMax != varMax ){
         d->varMax = varMax;
         if ( d->subtractorMog2() )
            d->subtractorMog2()->setVarMax(varMax);
         emit varMaxChanged();
    }
}

/*!
  \qmlproperty double BackgroundSubtractorMog2::varThreshold

  Threshold on the squared Mahalanobis distance to decide whether it is well described by the background model.
  Defaults to 16.
 */
float QBackgroundSubtractorMog2::varThreshold() const{
    Q_D(const QBackgroundSubtractorMog2);
    return d->varThreshold;
}

void QBackgroundSubtractorMog2::setVarThreshold(float varThreshold){
    Q_D(QBackgroundSubtractorMog2);
    if ( d->varThreshold != varThreshold ){
         d->varThreshold = varThreshold;
         if ( d->subtractorMog2() )
            d->subtractorMog2()->setVarThreshold(varThreshold);
         emit varThresholdChanged();
    }
}

/*!
  \qmlproperty double BackgroundSubtractorMog2::varThresholdGen

  Threshold for the squared Mahalanobis distance that helps decide when a sample is close to the existing components.
  Defaults to 9.
 */
float QBackgroundSubtractorMog2::varThresholdGen() const{
    Q_D(const QBackgroundSubtractorMog2);
    return d->varThresholdGen;
}

void QBackgroundSubtractorMog2::setVarThresholdGen(float varThresholdGen){
    Q_D(QBackgroundSubtractorMog2);
    if ( d->varThresholdGen != varThresholdGen ){
         d->varThresholdGen = varThresholdGen;
         if ( d->subtractorMog2() )
            d->subtractorMog2()->setVarThresholdGen(varThresholdGen);
         emit varThresholdGenChanged();
    }
}


/*!
  \qmlmethod BackgroundSubtractorMog2::staticLoad(string key)

  Loads the BackgroundSubtractorMog2 state from the given \a key.
 */
void QBackgroundSubtractorMog2::staticLoad(const QString &id){
    Q_D(QBackgroundSubtractorMog2);
    QStaticContainer* container = QStaticContainer::grabFromContext(this);
    d->m_subtractorMog2 = container->get<Ptr<BackgroundSubtractorMOG2> >(id);
    if ( !d->m_subtractorMog2 ){
        d->m_subtractorMog2 = d->createSubtractor();
        container->set< Ptr<BackgroundSubtractorMOG2> >(id, d->m_subtractorMog2);
    }
    QMatFilter::transform();
}
