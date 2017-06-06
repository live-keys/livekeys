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

#ifndef QDENOISETVL1_H
#define QDENOISETVL1_H

#include "qmatfilter.h"

class QDenoiseTvl1 : public QMatFilter{

    Q_OBJECT
    Q_PROPERTY(int    bufferSize READ bufferSize WRITE setBufferSize NOTIFY bufferSizeChanged)
    Q_PROPERTY(double lambda     READ lambda     WRITE setLambda     NOTIFY lambdaChanged)
    Q_PROPERTY(int    nIters     READ nIters     WRITE setNIters     NOTIFY nItersChanged)

public:
    explicit QDenoiseTvl1(QQuickItem *parent = 0);
    ~QDenoiseTvl1();

    virtual void transform(cv::Mat &in, cv::Mat &out);

    bool bufferSize() const;
    int lambda() const;
    int nIters() const;

    void setBufferSize(int bufferSize);
    void setLambda(int lambda);
    void setNIters(int nIters);

signals:
    void bufferSizeChanged();
    void lambdaChanged();
    void nItersChanged();

private:
    void trimBuffer(int size);

private:
    std::vector<cv::Mat> m_matBuffer;
    int m_bufferSize;
    double m_lambda;
    int m_nIters;
};

inline bool QDenoiseTvl1::bufferSize() const{
    return m_bufferSize;
}

inline int QDenoiseTvl1::lambda() const{
    return m_lambda;
}

inline int QDenoiseTvl1::nIters() const{
    return m_nIters;
}

inline void QDenoiseTvl1::setLambda(int lambda){
    if ( m_lambda != lambda ){
        m_lambda = lambda;
        emit lambdaChanged();
        QMatFilter::transform();
    }
}

inline void QDenoiseTvl1::setNIters(int nIters){
    if ( m_nIters != nIters ){
        m_nIters = nIters;
        emit nItersChanged();
        QMatFilter::transform();
    }
}

#endif // QDENOISETVL1_H
