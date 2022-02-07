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

#ifndef QWRITABLEMAT_H
#define QWRITABLEMAT_H

#include "qlcvcoreglobal.h"
#include "opencv2/core.hpp"
#include <QObject>
#include <QSize>

class QMat;

class Q_LCVCORE_EXPORT QWritableMat : public QObject{

    Q_OBJECT

public:
    explicit QWritableMat(QObject *parent = nullptr);
    QWritableMat(cv::Mat* mat, QObject* parent = nullptr);
    virtual ~QWritableMat();

    const cv::Mat& internal() const;
    cv::Mat& internal();

public slots:
    QByteArray buffer();
    int channels() const;
    int depth() const;
    QSize dimensions() const;
    QMat* toMat() const;

    void fill(const QColor& color, QMat* mask);

private:
    cv::Mat* m_internal;
};

#endif // QWRITABLEMAT_H
