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

#ifndef QBACKGROUNDSUBTRACTOR_HPP
#define QBACKGROUNDSUBTRACTOR_HPP

#include <QQuickItem>
#include "qmatfilter.h"
#include "opencv2/video.hpp"

class QBackgroundSubtractorPrivate{

public:
    QBackgroundSubtractorPrivate();
    virtual ~QBackgroundSubtractorPrivate();

    virtual void deleteSubtractor();
    virtual cv::BackgroundSubtractor* subtractor();

    const QString& stateId() const;
    void setStateId(const QString& id);

    double learningRate() const;
    void setLearningRate(double rate);

private:
    QString m_stateId;
    double m_learningRate;

};

class QBackgroundSubtractor : public QMatFilter{

    Q_OBJECT
    Q_PROPERTY(QString stateId      READ stateId      WRITE setStateId      NOTIFY stateIdChanged)
    Q_PROPERTY(double  learningRate READ learningRate WRITE setLearningRate NOTIFY learningRateChanged)

public:
    explicit QBackgroundSubtractor(QBackgroundSubtractorPrivate *d_ptr = 0, QQuickItem *parent = 0);
    virtual ~QBackgroundSubtractor();

    const QString& stateId() const;
    void setStateId(const QString& id);

    double learningRate() const;
    void setLearningRate(double rate);

    virtual void transform(cv::Mat& in, cv::Mat& out);

public slots:
    void reset();

signals:
    void stateIdChanged();
    void learningRateChanged();

protected:
    QBackgroundSubtractorPrivate* const d_ptr;

private:
    QBackgroundSubtractor(const QBackgroundSubtractor& other);
    QBackgroundSubtractor& operator= (const QBackgroundSubtractor& other);

    Q_DECLARE_PRIVATE(QBackgroundSubtractor)

};

#endif // QBACKGROUNDSUBTRACTOR_HPP
