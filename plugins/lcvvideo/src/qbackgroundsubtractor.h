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

#ifndef QBACKGROUNDSUBTRACTOR_H
#define QBACKGROUNDSUBTRACTOR_H

#include <QQuickItem>
#include "qlcvvideoglobal.h"
#include "qmatfilter.h"
#include "opencv2/video.hpp"

class Q_LCVVIDEO_EXPORT QBackgroundSubtractorPrivate{

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

class QBackgroundSubtractor : public QMatFilter{

    Q_OBJECT
    Q_PROPERTY(double learningRate READ learningRate WRITE setLearningRate NOTIFY learningRateChanged)

public:
    explicit QBackgroundSubtractor(QBackgroundSubtractorPrivate *d_ptr = 0, QQuickItem *parent = 0);
    virtual ~QBackgroundSubtractor();

    double learningRate() const;
    void setLearningRate(double rate);

    virtual void transform(const cv::Mat& in, cv::Mat& out);

signals:
    void learningRateChanged();

protected:
    QBackgroundSubtractorPrivate* const d_ptr;

private:
    QBackgroundSubtractor(const QBackgroundSubtractor& other);
    QBackgroundSubtractor& operator= (const QBackgroundSubtractor& other);

    Q_DECLARE_PRIVATE(QBackgroundSubtractor)

};

#endif // QBACKGROUNDSUBTRACTOR_H
