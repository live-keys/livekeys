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

#ifndef QBACKGROUNDSUBTRACTORKNN_HPP
#define QBACKGROUNDSUBTRACTORKNN_HPP

#include <QQuickItem>
#include "qbackgroundsubtractor.h"

class QBackgroundSubtractorKnnPrivate;

class QBackgroundSubtractorKnn : public QBackgroundSubtractor{

    Q_OBJECT
    Q_PROPERTY(bool   detectShadows   READ detectShadows   WRITE setDetectShadows   NOTIFY detectShadowsChanged)
    Q_PROPERTY(double dist2Threshold  READ dist2Threshold  WRITE setDist2Threshold  NOTIFY dist2ThresholdChanged)
    Q_PROPERTY(int    history         READ history         WRITE setHistory         NOTIFY historyChanged)
    Q_PROPERTY(int    knnSamples      READ knnSamples      WRITE setKnnSamples      NOTIFY knnSamplesChanged)
    Q_PROPERTY(int    nSamples        READ nSamples        WRITE setNSamples        NOTIFY nSamplesChanged)
    Q_PROPERTY(double shadowThreshold READ shadowThreshold WRITE setShadowThreshold NOTIFY shadowThresholdChanged)
    Q_PROPERTY(int    shadowValue     READ shadowValue     WRITE setShadowValue     NOTIFY shadowValueChanged)

public:
    explicit QBackgroundSubtractorKnn(QQuickItem *parent = 0);
    ~QBackgroundSubtractorKnn();

    bool detectShadows() const;
    void setDetectShadows(bool detectShadows);

    double dist2Threshold() const;
    void setDist2Threshold(double dist2Threshold);

    int history() const;
    void setHistory(int history);

    int knnSamples() const;
    void setKnnSamples(int knnSamples);

    int nSamples() const;
    void setNSamples(int nSamples);

    double shadowThreshold() const;
    void setShadowThreshold(double shadowThreshold);

    int shadowValue() const;
    void setShadowValue(int shadowValue);

signals:
    void detectShadowsChanged();
    void dist2ThresholdChanged();
    void historyChanged();
    void knnSamplesChanged();
    void nSamplesChanged();
    void shadowThresholdChanged();
    void shadowValueChanged();

private:
    QBackgroundSubtractorKnn(const QBackgroundSubtractorKnn& other);
    QBackgroundSubtractorKnn& operator= (const QBackgroundSubtractorKnn& other);

    QBackgroundSubtractorKnnPrivate* const d_ptr;

    Q_DECLARE_PRIVATE(QBackgroundSubtractorKnn)

};

#endif // QBACKGROUNDSUBTRACTORKNN_HPP
