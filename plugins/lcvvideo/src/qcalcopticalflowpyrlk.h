#ifndef QCALCOPTICALFLOWPYRLK_HPP
#define QCALCOPTICALFLOWPYRLK_HPP

#include <QQuickItem>
#include "qmatfilter.h"

class QCalcOpticalFlowPyrLKPrivate;

class QCalcOpticalFlowPyrLK : public QMatFilter{

    Q_OBJECT
    Q_PROPERTY(QString stateId         READ stateId         WRITE setStateId         NOTIFY stateIdChanged)
    Q_PROPERTY(QSize   winSize         READ winSize         WRITE setWinSize         NOTIFY winSizeChanged)
    Q_PROPERTY(int     maxLevel        READ maxLevel        WRITE setMaxLevel        NOTIFY maxLevelChanged)
    Q_PROPERTY(double  minEigThreshold READ minEigThreshold WRITE setMinEigThreshold NOTIFY minEigThresholdChanged)

public:
    explicit QCalcOpticalFlowPyrLK(QQuickItem *parent = 0);
    ~QCalcOpticalFlowPyrLK();

    QSize winSize() const;
    void setWinSize(const QSize& winSize);

    int maxLevel() const;
    void setMaxLevel(int maxLevel);

    double minEigThreshold() const;
    void setMinEigThreshold(double minEigThreshold);

    const QString& stateId() const;
    void setStateId(const QString& id);

    virtual void transform(cv::Mat& in, cv::Mat& out);
    virtual QSGNode *updatePaintNode(QSGNode *node, UpdatePaintNodeData *nodeData);

public slots:
    void addPoint(const QPoint& point);
    QList<QPoint> points();
    int totalPoints() const;

signals:
    void winSizeChanged();
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
