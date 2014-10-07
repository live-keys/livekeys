#ifndef QCALCOPTICALFLOWPYRLK_HPP
#define QCALCOPTICALFLOWPYRLK_HPP

#include <QQuickItem>
#include "QMatFilter.hpp"

/**
 * @page qml_calcopticalflowpyrlk CalcOpticalFlowPyrLK
 * @ingroup qml_video
 *
 * Calculate an optical flow for a sparse feature set using the iterative Lucas-Kanade method with pyramids. The function implements a sparse iterative version of
 * the Lucas-Kanade optical flow in pyramids. See http://docs.opencv.org/modules/video/doc/motion_analysis_and_object_tracking.html for details.
 *
 * Parameters :
 *
 *  - <b>winSize</b> : size of the search window at each pyramid level.
 *  - <b>maxLevel</b> : 0-based maximal pyramid level number; if set to 0, pyramids are not used (single level), if set to 1, two levels are used, and so on; if pyramids are passed to input then algorithm will use as many levels as pyramids have but no more than ``maxLevel``.
 *  - <b>criteria</b> : parameter, specifying the termination criteria of the iterative search algorithm (after the specified maximum number of iterations  ``criteria.maxCount``  or when the search window moves by less than  ``criteria.epsilon``.
 *  - <b>flags</b> : operation flags:
 *     - **OPTFLOW_USE_INITIAL_FLOW** uses initial estimations, stored in ``nextPts``; if the flag is not set, then ``prevPts`` is copied to ``nextPts`` and is considered the initial estimate.
 *     - **OPTFLOW_LK_GET_MIN_EIGENVALS** use minimum eigen values as an error measure (see ``minEigThreshold`` description); if the flag is not set, then L1 distance between patches around the original and a moved point, divided by number of pixels in a window, is used as a error measure.
 *  - <b>minEigThreshold</b> : the algorithm calculates the minimum eigen value of a 2x2 normal matrix of optical flow equations (this matrix is called a spatial gradient matrix in [Bouguet00]_), divided by number of pixels in a window; if this value is less than ``minEigThreshold``, then a corresponding feature is filtered out and its flow is not processed, so it allows to remove bad points and get a performance boost.
 *
 * Note that this element manages it's points automatically. You can add a point by using the addPoint() function, or you can get a list of all the points by using the
 * points() getter.
 *
 * @code
 * var points = lkflow.points();
 * @endcode
 *
 * Sample available at [samples/video/lktracking.qml](@ref qml_sample_video_lktracking).
 */

class QCalcOpticalFlowPyrLKPrivate;

/**
 * @brief The QCalcOpticalFlowPyrLK class
 * @author Dinu SV
 * @version 1.0.0
 * @ingroup cpp_video
 *
 * QML documentation for this class is available [here](@ref qml_calcopticalflowpyrlk).
 */
class QCalcOpticalFlowPyrLK : public QMatFilter{

    Q_OBJECT
    Q_PROPERTY(QSize   winSize         READ winSize         WRITE setWinSize         NOTIFY winSizeChanged)
    Q_PROPERTY(int     maxLevel        READ maxLevel        WRITE setMaxLevel        NOTIFY maxLevelChanged)
    Q_PROPERTY(int     flags           READ flags           WRITE setFlags           NOTIFY flagsChanged)
    Q_PROPERTY(double  minEigThreshold READ minEigThreshold WRITE setMinEigThreshold NOTIFY minEigThresholdChanged)
    Q_PROPERTY(QString stateId         READ stateId         WRITE setStateId         NOTIFY stateIdChanged)

public:
    explicit QCalcOpticalFlowPyrLK(QQuickItem *parent = 0);
    ~QCalcOpticalFlowPyrLK();

    QSize winSize() const;
    void setWinSize(const QSize& winSize);

    int maxLevel() const;
    void setMaxLevel(int maxLevel);

    int flags() const;
    void setFlags(int flags);

    double minEigThreshold() const;
    void setMinEigThreshold(double minEigThreshold);

    const QString& stateId() const;
    void setStateId(const QString& id);

    virtual void transform(cv::Mat& in, cv::Mat&);
    virtual QSGNode *updatePaintNode(QSGNode *node, UpdatePaintNodeData *nodeData);

public slots:
    void addPoint(const QPoint& point);
    QList<QPoint> points();
    int totalPoints() const;

signals:
    void winSizeChanged();
    void flagsChanged();
    void maxLevelChanged();
    void minEigThresholdChanged();
    void stateIdChanged();

private:
    QCalcOpticalFlowPyrLK(const QCalcOpticalFlowPyrLK& other);
    QCalcOpticalFlowPyrLK& operator= (const QCalcOpticalFlowPyrLK& other);

    QCalcOpticalFlowPyrLKPrivate* const d_ptr;

    Q_DECLARE_PRIVATE(QCalcOpticalFlowPyrLK)

};

#endif // QCALCOPTICALFLOWPYRLK_HPP
