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

#ifndef QBACKGROUNDSUBTRACTORMOG2_H
#define QBACKGROUNDSUBTRACTORMOG2_H

#include <QQuickItem>
#include "qbackgroundsubtractor.h"

class QBackgroundSubtractorMog2Private;

class QBackgroundSubtractorMog2 : public QBackgroundSubtractor{

    Q_OBJECT
    Q_PROPERTY(QMat* backgroundModel  READ backgroundModel)
    Q_PROPERTY(int   history          READ history          WRITE setHistory          NOTIFY historyChanged)
    Q_PROPERTY(int   nmixtures        READ nmixtures        WRITE setNmixtures        NOTIFY nmixturesChanged)
    Q_PROPERTY(int   nShadowDetection READ nShadowDetection WRITE setNShadowDetection NOTIFY nShadowDetectionChanged)
    Q_PROPERTY(bool  detectShadows    READ detectShadows    WRITE setDetectShadows    NOTIFY detectShadowsChanged)
    Q_PROPERTY(float backgroundRatio  READ backgroundRatio  WRITE setBackgroundRatio  NOTIFY backgroundRatioChanged)
    Q_PROPERTY(float ct               READ ct               WRITE setCt               NOTIFY ctChanged)
    Q_PROPERTY(float tau              READ tau              WRITE setTau              NOTIFY tauChanged)
    Q_PROPERTY(float varInit          READ varInit          WRITE setVarInit          NOTIFY varInitChanged)
    Q_PROPERTY(float varMin           READ varMin           WRITE setVarMin           NOTIFY varMinChanged)
    Q_PROPERTY(float varMax           READ varMax           WRITE setVarMax           NOTIFY varMaxChanged)
    Q_PROPERTY(float varThreshold     READ varThreshold     WRITE setVarThreshold     NOTIFY varThresholdChanged)
    Q_PROPERTY(float varThresholdGen  READ varThresholdGen  WRITE setVarThresholdGen  NOTIFY varThresholdGenChanged)

public:
    explicit QBackgroundSubtractorMog2(QQuickItem *parent = 0);
    virtual ~QBackgroundSubtractorMog2();

    QMat* backgroundModel();

    int history() const;
    void setHistory(int history);

    int nmixtures() const;
    void setNmixtures(int nmixtures);

    int nShadowDetection() const;
    void setNShadowDetection(int nShadowDetection);

    bool detectShadows() const;
    void setDetectShadows(bool detectShadows);

    float backgroundRatio() const;
    void setBackgroundRatio(float backgroundRatio);

    float ct() const;
    void setCt(float ct);

    float tau() const;
    void setTau(float tau);

    float varInit() const;
    void setVarInit(float varInit);

    float varMin() const;
    void setVarMin(float varMin);

    float varMax() const;
    void setVarMax(float varMax);

    float varThreshold() const;
    void setVarThreshold(float varThreshold);

    float varThresholdGen() const;
    void setVarThresholdGen(float varThresholdGen);

signals:
    void historyChanged();
    void nmixturesChanged();
    void nShadowDetectionChanged();
    void detectShadowsChanged();
    void backgroundRatioChanged();
    void ctChanged();
    void tauChanged();
    void varInitChanged();
    void varMinChanged();
    void varMaxChanged();
    void varThresholdChanged();
    void varThresholdGenChanged();

public slots:
    void staticLoad(const QString& id);

private:
    QBackgroundSubtractorMog2(const QBackgroundSubtractorMog2& other);
    QBackgroundSubtractorMog2& operator= (const QBackgroundSubtractorMog2& other);

    QBackgroundSubtractorMog2Private* const d_ptr;

    Q_DECLARE_PRIVATE(QBackgroundSubtractorMog2)

};

#endif // QBACKGROUNDSUBTRACTORMOG2_H
