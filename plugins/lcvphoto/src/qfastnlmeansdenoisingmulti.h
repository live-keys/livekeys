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

#ifndef QFASTNLMEANSDENOISINGMULTI_H
#define QFASTNLMEANSDENOISINGMULTI_H

#include <QList>
#include "qfastnlmeansdenoising.h"

using namespace cv;

class QFastNlMeansDenoisingMulti : public QFastNlMeansDenoising{

    Q_OBJECT
    Q_PROPERTY(int temporalWindowSize READ temporalWindowSize WRITE setTemporalWindowSize NOTIFY temporalWindowSizeChanged)

public:
    explicit QFastNlMeansDenoisingMulti(QQuickItem *parent = 0);
    ~QFastNlMeansDenoisingMulti();

    virtual void transform(cv::Mat &in, cv::Mat &out);

    int temporalWindowSize() const;

    void setTemporalWindowSize(int temporalWindowSize);

signals:
    void temporalWindowSizeChanged();

private:
    void trimFrameHistory(std::size_t size);

private:
    int m_temporalWindowSize;
    std::vector<Mat> m_frameHistory;
};

inline int QFastNlMeansDenoisingMulti::temporalWindowSize() const{
    return m_temporalWindowSize;
}

#endif // QFASTNLMEANSDENOISINGMULTI_H
