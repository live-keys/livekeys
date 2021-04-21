/****************************************************************************
**
** Copyright (C) 2014-2019 Dinu SV.
** (contact: mail@dinusv.com)
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

#include "qcalcopticalflowpyrlk.h"
#include "opencv2/video/tracking.hpp"
#include "live/staticcontainer.h"

using namespace cv;

/// \private
class QCalcOpticalFlowPointState{

public:
    std::vector<Point2f> currentPoints;
    std::vector<Point2f> prevPoints;
    std::vector<uchar>   status;
    std::vector<float>   err;
};

/// \private
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
        cvtColor(input, gray, cv::COLOR_BGR2GRAY);

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

/**
 *\class QCalcOpticalFlowPyrLK
 *\ingroup plugin-lcvvideo
 *\internal
 *\brief Calculates the sparse optical flow.
 */

/**
 *\brief QCalcOpticalFlowPyrLK constructor
 *
 *Parameters:
 *\a parent
 */
QCalcOpticalFlowPyrLK::QCalcOpticalFlowPyrLK(QQuickItem *parent)
    : QMatFilter(parent)
    , d_ptr(new QCalcOpticalFlowPyrLKPrivate){

}

/**
 *\brief QCalcOpticalFlowPyrLK destructor
 */
QCalcOpticalFlowPyrLK::~QCalcOpticalFlowPyrLK(){
    delete d_ptr;
}

/**
 *\brief Adds a \a point to the vector of points that are currently tracked.
 *\sa CalcOpticalFlowPyrLK::addPoint
 */
void QCalcOpticalFlowPyrLK::addPoint(const QPoint& point){
    Q_D(QCalcOpticalFlowPyrLK);
    d->addPoint(Point(point.x(), point.y()));
}

/**
 *\brief Returns the total number of points that are currently tracked as a list of points.
 *\sa CalcOpticalFlowPyrLK::points()
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

/**
 *\brief Returns the total number of points that are currently tracked.
 */
int QCalcOpticalFlowPyrLK::totalPoints() const{
    Q_D(const QCalcOpticalFlowPyrLK);
    return (int)d->totalPoints();
}

void QCalcOpticalFlowPyrLK::staticLoad(const QString &id){
    Q_D(QCalcOpticalFlowPyrLK);
    lv::StaticContainer* container = lv::StaticContainer::grabFromContext(this);
    d->pointState = container->get<QCalcOpticalFlowPointState>(id);
    if ( !d->pointState ){
        d->pointState = new QCalcOpticalFlowPointState;
        container->set<QCalcOpticalFlowPointState>(id, d->pointState);
    }
    QMatFilter::transform();
}

/**
 *\property QCalcOpticalFlowPyrLK::winSize
 *\sa CalcOpticalFlowPyrLK::winSize
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

/**
 *\property QCalcOpticalFlowPyrLK::maxLevel
 *\sa CalcOpticalFlowPyrLK::maxLevel
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


/**
 *\property QCalcOpticalFlowPyrLK::minEigThreshold
 *\sa CalcOpticalFlowPyrLK::minEigThreshold
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

/**
 *\fn virtual void QCalcOpticalFlowPyrLK::transform(const cv::Mat& in, cv::Mat& out)
 *\brief Filtering function.
 *
 *Parameters :
 *\a in
 *\a out
 */
void QCalcOpticalFlowPyrLK::transform(const Mat& in, Mat&){
    Q_D(QCalcOpticalFlowPyrLK);
    d->calculateFlow(in);
}

/**
 *\fn virtual QSGNode *QCalcOpticalFlowPyrLK::updatePaintNode(QSGNode *node, UpdatePaintNodeData *nodeData)
 *\brief Draws the points on the output matrix.
 *
 *Parameters :
 *\a node
 *\a nodeData
 */
QSGNode* QCalcOpticalFlowPyrLK::updatePaintNode(QSGNode* node, QQuickItem::UpdatePaintNodeData*nodeData){
    Q_D(QCalcOpticalFlowPyrLK);
    d->draw( *(inputMat()->internalPtr()), *(output()->internalPtr()) );

    return QMatDisplay::updatePaintNode(node, nodeData);
}
