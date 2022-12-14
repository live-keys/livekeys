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

#ifndef QBACKGROUNDSUBTRACTOR_H
#define QBACKGROUNDSUBTRACTOR_H

#include <QQuickItem>
#include "qlcvvideoglobal.h"
#include "qmat.h"
#include "opencv2/video.hpp"

/// \private
class QBackgroundSubtractorPrivate{

public:
    QBackgroundSubtractorPrivate();
    virtual ~QBackgroundSubtractorPrivate();

    virtual cv::BackgroundSubtractor* subtractor();

    double learningRate() const;
    void setLearningRate(double rate);

private:
    QString m_stateId;
    double m_learningRate;
};

class Q_LCVVIDEO_EXPORT QBackgroundSubtractor : public QObject{

    Q_OBJECT
    Q_PROPERTY(double learningRate READ learningRate WRITE setLearningRate NOTIFY learningRateChanged)

public:
    explicit QBackgroundSubtractor(QBackgroundSubtractorPrivate *d_ptr = 0, QQuickItem *parent = 0);
    virtual ~QBackgroundSubtractor();

    double learningRate() const;
    /// \private
    void setLearningRate(double rate);

    virtual void transform(const cv::Mat& in, cv::Mat& out);

public slots:
    QMat* subtract(QMat* input);

signals:
    /// \private
    void learningRateChanged();

protected:
    /// \private
    QBackgroundSubtractorPrivate* const d_ptr;

private:
    QBackgroundSubtractor(const QBackgroundSubtractor& other);
    QBackgroundSubtractor& operator= (const QBackgroundSubtractor& other);

    Q_DECLARE_PRIVATE(QBackgroundSubtractor)

};

#endif // QBACKGROUNDSUBTRACTOR_H
