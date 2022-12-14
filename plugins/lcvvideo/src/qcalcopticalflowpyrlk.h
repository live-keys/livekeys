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

#ifndef QCALCOPTICALFLOWPYRLK_H
#define QCALCOPTICALFLOWPYRLK_H

#include <QQuickItem>
#include "qmat.h"

class QCalcOpticalFlowPyrLKPrivate;

/// \private
class QCalcOpticalFlowPyrLK : public QObject{

    Q_OBJECT
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

    virtual void transform(const cv::Mat& in, cv::Mat& out);

public slots:
    QMat* draw(QMat* input);
    QMat* transform(QMat* input);

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

#endif // QCALCOPTICALFLOWPYRLK_H
