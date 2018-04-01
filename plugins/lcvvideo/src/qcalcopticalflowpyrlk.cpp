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

#include "qcalcopticalflowpyrlk.h"
#include "opencv2/video/tracking.hpp"
#include "qstaticcontainer.h"

using namespace cv;

/*!
  \qmltype CalcOpticalFlowPyrLK
  \instantiates QCalcOpticalFlowPyrLK
  \inqmlmodule lcvvideo
  \inherits MatFilter
  \brief Sparse optical flow filter.

  Calculate an optical flow for a sparse feature set using the iterative Lucas-Kanade method with pyramids. The function
  implements a sparse iterative version of the Lucas-Kanade optical flow in pyramids.

  See http://docs.opencv.org/modules/video/doc/motion_analysis_and_object_tracking.html for details.

  Note that this element manages it's points automatically. You can add a point by using the addPoint() function, or
  you can get a list of all the points by using the points() getter.

  \code
  var points = lkflow.points()
  \endcode

  A sample is available in \b{samples/video/lktracking.qml} :

  \quotefile video/lktracking.qml
*/

class QCalcOpticalFlowPointState{

public:
    std::vector<Point2f> currentPoints;
    std::vector<Point2f> prevPoints;
    std::vector<uchar>   status;
    std::vector<float>   err;
};

class QCalcOpticalFlowPyrLKPrivate{

public:
    QCalcOpticalFlowPyrLKPrivate();
    ~QCalcOpticalFlowPyrLKPrivate();

    void calculateFlow(const Mat &input);
    void draw(cv::Mat& frame, cv::Mat& m);
    void addPoint(const cv::Point& p);

    size_t totalPoints() const;

public:
    cv::Mat              gray;
    cv::Mat              prevGray;
    cv::Size             winSize;
    cv::Scalar           pointColor;
    cv::TermCriteria     termcrit;
    int                  maxLevel;
    int                  flags;
    double               minEigThreshold;

    QCalcOpticalFlowPointState* pointState;
};


// QCalcOpticalFlowPyrLKPrivate Implementation
// -------------------------------------------

QCalcOpticalFlowPyrLKPrivate::QCalcOpticalFlowPyrLKPrivate()
    : winSize(21, 21)
    , pointColor(255, 255, 0)
    , termcrit(TermCriteria::COUNT+TermCriteria::EPS, 30, 0.01)
    , maxLevel(3)
    , flags(0)
    , minEigThreshold(1e-4)
    , pointState(0){

}

QCalcOpticalFlowPyrLKPrivate::~QCalcOpticalFlowPyrLKPrivate(){
}

void QCalcOpticalFlowPyrLKPrivate::calculateFlow(const cv::Mat& input){
    if ( !pointState ){
        qWarning("This item requires staticLoading.");
        return;
    }

    if ( input.channels() == 1 )
        gray = input;
    else
        cvtColor(input, gray, CV_BGR2GRAY);

    if ( !pointState->currentPoints.empty() ){

        std::swap(pointState->currentPoints, pointState->prevPoints);
        if ( prevGray.empty() )
            gray.copyTo(prevGray);
        else {

            calcOpticalFlowPyrLK(
                        prevGray, gray, pointState->prevPoints, pointState->currentPoints,
                        pointState->status, pointState->err, winSize, maxLevel, termcrit, flags, minEigThreshold);

            gray.copyTo(prevGray);

            size_t k = 0;
            for ( size_t i = 0; i < pointState->currentPoints.size(); ++i ){
                if ( !pointState->status[i] )
                    continue;
                pointState->currentPoints[k++] = pointState->currentPoints[i];
            }
            pointState->currentPoints.resize(k);
        }
    }
}

void QCalcOpticalFlowPyrLKPrivate::draw(cv::Mat& frame, cv::Mat& m){
    if ( !pointState )
        return;

    frame.copyTo(m);
    for ( size_t i = 0; i < pointState->currentPoints.size(); ++i )
        circle(m, pointState->currentPoints[i], 3, pointColor, -1, 8);
}

void QCalcOpticalFlowPyrLKPrivate::addPoint(const cv::Point& p){
    if ( pointState )
        pointState->currentPoints.push_back(p);
}

size_t QCalcOpticalFlowPyrLKPrivate::totalPoints() const{
    if ( pointState )
        return pointState->currentPoints.size();
    return 0;
}


// QCalcOpticalFlowPyrLK Implementation
// ------------------------------------

/*!
   \class QCalcOpticalFlowPyrLK
   \inmodule lcvvideo_cpp
   \internal
   \brief Calculates the sparse optical flow.
 */

/*!
  \brief QCalcOpticalFlowPyrLK constructor

  Parameters:
  \a parent
 */
QCalcOpticalFlowPyrLK::QCalcOpticalFlowPyrLK(QQuickItem *parent)
    : QMatFilter(parent)
    , d_ptr(new QCalcOpticalFlowPyrLKPrivate){

}

/*!
  \brief QCalcOpticalFlowPyrLK destructor
 */
QCalcOpticalFlowPyrLK::~QCalcOpticalFlowPyrLK(){
    delete d_ptr;
}


/*!
  \qmlmethod CalcOpticalFlowPyrLK::addPoint(Point p)

  Add a \a point to track.
 */

/*!
  \brief Adds a \a point to the vector of points that are currently tracked.
  \sa CalcOpticalFlowPyrLK::addPoint
 */
void QCalcOpticalFlowPyrLK::addPoint(const QPoint& point){
    Q_D(QCalcOpticalFlowPyrLK);
    d->addPoint(Point(point.x(), point.y()));
}

/*!
  \qmlmethod list<Point> CalcOpticalFlowPyrLK::points()

  This method retrieves a copy of the list of points that are currently traked.
 */

/*!
  \brief Returns the total number of points that are currently tracked as a list of points.
  \sa CalcOpticalFlowPyrLK::points()
 */
QList<QPoint> QCalcOpticalFlowPyrLK::points(){
    Q_D(QCalcOpticalFlowPyrLK);
    QList<QPoint> base;

    QCalcOpticalFlowPointState* ps = d->pointState;
    if (!ps)
        return base;


    for ( size_t i = 0; i < ps->currentPoints.size(); ++i ){
        base.append(QPoint(ps->currentPoints[i].x, ps->currentPoints[i].y));
    }
    return base;
}

/*!
  \qmlmethod int CalcOpticalFlowPyrLK::totalPoints()

  Returns the total number of points that are currently tracked.
 */

/*!
  \brief Returns the total number of points that are currently tracked.
 */
int QCalcOpticalFlowPyrLK::totalPoints() const{
    Q_D(const QCalcOpticalFlowPyrLK);
    return (int)d->totalPoints();
}



/*!
  \qmlmethod CalcOpticalFlowPyrLK::staticLoad(string key)

  Loads the CalcOpticalFlowPyrLK state from the given \a key.
 */

void QCalcOpticalFlowPyrLK::staticLoad(const QString &id){
    Q_D(QCalcOpticalFlowPyrLK);
    QStaticContainer* container = QStaticContainer::grabFromContext(this);
    d->pointState = container->get<QCalcOpticalFlowPointState>(id);
    if ( !d->pointState ){
        d->pointState = new QCalcOpticalFlowPointState;
        container->set<QCalcOpticalFlowPointState>(id, d->pointState);
    }
    QMatFilter::transform();
}

/*!
  \property QCalcOpticalFlowPyrLK::winSize
  \sa CalcOpticalFlowPyrLK::winSize
 */

/*!
  \qmlproperty Size CalcOpticalFlowPyrLK::winSize

  Size of the search window at each pyramid level.
 */


QSize QCalcOpticalFlowPyrLK::winSize() const{
    Q_D(const QCalcOpticalFlowPyrLK);
    return QSize(d->winSize.width, d->winSize.height);
}

void QCalcOpticalFlowPyrLK::setWinSize(const QSize& winSize){
    Q_D(QCalcOpticalFlowPyrLK);
    if ( d->winSize.width != winSize.width() || d->winSize.height != winSize.height() ){
        d->winSize = Size(winSize.width(), winSize.height());
        emit winSizeChanged();
    }
}


/*!
  \qmlproperty int CalcOpticalFlowPyrLK::maxLevel

  0-based maximal pyramid level number; if set to 0, pyramids are not used (single level), if set to 1, two levels are
  used, and so on; if pyramids are passed to input then algorithm will use as many levels as pyramids have but no more
  than 'maxLevel'.
 */

/*!
  \property QCalcOpticalFlowPyrLK::maxLevel
  \sa CalcOpticalFlowPyrLK::maxLevel
 */

int QCalcOpticalFlowPyrLK::maxLevel() const{
    Q_D(const QCalcOpticalFlowPyrLK);
    return d->maxLevel;
}

void QCalcOpticalFlowPyrLK::setMaxLevel(int maxLevel){
    Q_D(QCalcOpticalFlowPyrLK);
    if (d->maxLevel != maxLevel){
        d->maxLevel = maxLevel;
        emit maxLevelChanged();
    }
}


/*!
  \property QCalcOpticalFlowPyrLK::minEigThreshold
  \sa CalcOpticalFlowPyrLK::minEigThreshold
 */

/*!
  \qmlproperty real CalcOpticalFlowPyrLK::minEigThreshold

  The algorithm calculates the minimum eigen value of a 2x2 normal matrix of optical flow equations (this matrix is
  called a spatial gradient matrix in [Bouguet00]_), divided by number of pixels in a window; if this value is less than
  'minEigThreshold', then a corresponding feature is filtered out and its flow is not processed, so it allows to remove
  bad points and get a performance boost.
 */

double QCalcOpticalFlowPyrLK::minEigThreshold() const{
    Q_D(const QCalcOpticalFlowPyrLK);
    return d->minEigThreshold;
}

void QCalcOpticalFlowPyrLK::setMinEigThreshold(double minEigThreshold){
    Q_D(QCalcOpticalFlowPyrLK);
    if ( d->minEigThreshold != minEigThreshold ){
        d->minEigThreshold = minEigThreshold;
        emit minEigThresholdChanged();
    }
}

/*!
  \fn virtual void QCalcOpticalFlowPyrLK::transform(const cv::Mat& in, cv::Mat& out)
  \brief Filtering function.

  Parameters :
  \a in
  \a out
 */
void QCalcOpticalFlowPyrLK::transform(const Mat& in, Mat&){
    Q_D(QCalcOpticalFlowPyrLK);
    d->calculateFlow(in);
}

/*!
  \fn virtual QSGNode *QCalcOpticalFlowPyrLK::updatePaintNode(QSGNode *node, UpdatePaintNodeData *nodeData)
  \brief Draws the points on the output matrix.

  Parameters :
  \a node
  \a nodeData
 */
QSGNode* QCalcOpticalFlowPyrLK::updatePaintNode(QSGNode* node, QQuickItem::UpdatePaintNodeData*nodeData){
    Q_D(QCalcOpticalFlowPyrLK);
    d->draw( *(inputMat()->cvMat()), *(output()->cvMat()) );

    return QMatDisplay::updatePaintNode(node, nodeData);
}
