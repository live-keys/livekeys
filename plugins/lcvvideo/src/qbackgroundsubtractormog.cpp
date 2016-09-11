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

#include "qbackgroundsubtractormog.h"
#include "qstatecontainer.h"

using namespace cv;

/*!
  \qmltype BackgroundSubtractorMog
  \instantiates QBackgroundSubtractorMog
  \inqmlmodule lcvvideo
  \inherits MatFilter
  \brief Background subtractor

  Background subtractor
*/

class QBackgroundSubtractorMogPrivate : public QBackgroundSubtractorPrivate{

public:
    QBackgroundSubtractorMogPrivate();
    virtual ~QBackgroundSubtractorMogPrivate();

    virtual void deleteSubtractor();
    virtual BackgroundSubtractor* subtractor();

    BackgroundSubtractorMOG* subtractorMog();

public:
    int history;
    int nmixtures;
    double backgroundRatio;
    double noiseSigma;

private:
    BackgroundSubtractorMOG* createSubtractor();

private:
    BackgroundSubtractorMOG* m_subtractorMog;

};

// QBackgroundSubtractorMogPrivate Implementation
// -------------------------------------------
QBackgroundSubtractorMogPrivate::QBackgroundSubtractorMogPrivate()
    : QBackgroundSubtractorPrivate()
    , history(20)
    , nmixtures(5)
    , backgroundRatio(0.7)
    , noiseSigma(15)
    , m_subtractorMog(0){
}

QBackgroundSubtractorMogPrivate::~QBackgroundSubtractorMogPrivate(){
    if ( stateId() == "" ) // otherwise leave it to the QStateContiner
        delete m_subtractorMog;
}

BackgroundSubtractorMOG* QBackgroundSubtractorMogPrivate::createSubtractor(){
    return new BackgroundSubtractorMOG(history, nmixtures, backgroundRatio, noiseSigma);
}

void QBackgroundSubtractorMogPrivate::deleteSubtractor(){
    QStateContainer<BackgroundSubtractorMOG>& stateCont =
            QStateContainer<BackgroundSubtractorMOG>::instance();
    stateCont.registerState(stateId(), static_cast<BackgroundSubtractorMOG*>(0));
    delete m_subtractorMog;
    m_subtractorMog = 0;
}

BackgroundSubtractorMOG* QBackgroundSubtractorMogPrivate::subtractorMog(){
    if ( !m_subtractorMog ){
        if ( stateId() != "" ){
            QStateContainer<BackgroundSubtractorMOG>& stateCont =
                    QStateContainer<BackgroundSubtractorMOG>::instance();

            m_subtractorMog = stateCont.state(stateId());
            if ( m_subtractorMog == 0 ){
                m_subtractorMog = createSubtractor();
                stateCont.registerState(stateId(), m_subtractorMog);
            }
        } else {
            qWarning("QBackgroundSubtractorMog does not have a stateId assigned and cannot save "
                     "it\'s state over multiple compilations. Set a unique stateId to "
                     "avoid this problem.");
            m_subtractorMog = createSubtractor();
        }
    }
    return m_subtractorMog;
}

BackgroundSubtractor* QBackgroundSubtractorMogPrivate::subtractor(){
    return subtractorMog();
}

// QBackgroundSubtractorMog Implementation
// ------------------------------------

/*!
   \class QBackgroundSubtractorMog
   \inmodule lcvvideo_cpp
   \brief Subtracts background using MOG algorithm.
 */

/*!
  \brief QBackgroundSubtractorMog constructor

  Parameters:
  \a parent
 */
QBackgroundSubtractorMog::QBackgroundSubtractorMog(QQuickItem *parent)
    : QBackgroundSubtractor(new QBackgroundSubtractorMogPrivate, parent)
    , d_ptr(static_cast<QBackgroundSubtractorMogPrivate*>(QBackgroundSubtractor::d_ptr)){
}

/*!
  \brief QBackgroundSubtractorMog destructor
 */
QBackgroundSubtractorMog::~QBackgroundSubtractorMog(){
}

/*!
  \property QBackgroundSubtractorMog::history
  \sa BackgroundSubtractorMog::history
 */

/*!
  \qmlproperty int BackgroundSubtractorMog::history

  Length of the history. Defaults to 20.
 */
int QBackgroundSubtractorMog::history() const{
    Q_D(const QBackgroundSubtractorMog);
    return d->history;
}

void QBackgroundSubtractorMog::setHistory(int history){
    Q_D(QBackgroundSubtractorMog);
    if ( d->history != history ){
         d->history = history;
         if ( d->subtractorMog() )
            d->subtractorMog()->setInt("history", history);
         emit historyChanged();
    }
}

/*!
  \property QBackgroundSubtractorMog::nmixtures
  \sa BackgroundSubtractorMog::nmixtures
 */

/*!
  \qmlproperty int BackgroundSubtractorMog::nmixtures

  Number of Gaussian mixtures. Defaults to 5.
 */
int QBackgroundSubtractorMog::nmixtures() const{
    Q_D(const QBackgroundSubtractorMog);
    return d->nmixtures;
}

void QBackgroundSubtractorMog::setNmixtures(int nmixtures){
    Q_D(QBackgroundSubtractorMog);
    if ( d->nmixtures != nmixtures ){
         d->nmixtures = nmixtures;
         if ( d->subtractorMog() )
            d->subtractorMog()->setInt("nmixtures", nmixtures);
         emit nmixturesChanged();
    }
}

/*!
  \property QBackgroundSubtractorMog::backgroundRatio
  \sa BackgroundSubtractorMog::backgroundRatio
 */

/*!
  \qmlproperty double BackgroundSubtractorMog::backgroundRatio

  Background ratio. Defaults to 0.7.
 */
double QBackgroundSubtractorMog::backgroundRatio() const{
    Q_D(const QBackgroundSubtractorMog);
    return d->backgroundRatio;
}

void QBackgroundSubtractorMog::setBackgroundRatio(double backgroundRatio){
    Q_D(QBackgroundSubtractorMog);
    if ( d->backgroundRatio != backgroundRatio ){
         d->backgroundRatio = backgroundRatio;
         if ( d->subtractorMog() )
            d->subtractorMog()->setDouble("backgroundRatio", backgroundRatio);
         emit backgroundRatioChanged();
    }
}

/*!
  \property QBackgroundSubtractorMog::noiseSigma
  \sa BackgroundSubtractorMog::noiseSigma
 */

/*!
  \qmlproperty double BackgroundSubtractorMog::noiseSigma

  Noise strength. Defaults to 15.
 */
double QBackgroundSubtractorMog::noiseSigma() const{
    Q_D(const QBackgroundSubtractorMog);
    return d->noiseSigma;
}

void QBackgroundSubtractorMog::setNoiseSigma(double noiseSigma){
    Q_D(QBackgroundSubtractorMog);
    if ( d->noiseSigma != noiseSigma ){
         d->noiseSigma = noiseSigma;
         if ( d->subtractorMog() )
            d->subtractorMog()->setDouble("noiseSigma", noiseSigma);
         emit noiseSigmaChanged();
    }
}
