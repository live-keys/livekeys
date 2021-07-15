/****************************************************************************
**
** Copyright (C) 2014-2019 Dinu SV.
** (contact: mail@dinusv.com)
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

#ifndef QCALIBRATEROBERTSON_H
#define QCALIBRATEROBERTSON_H

#include <QObject>
#include <QQmlParserStatus>
#include "opencv2/photo.hpp"
#include "live/qmlobjectlist.h"
#include "qmat.h"

class QCalibrateRobertson : public QObject{

    Q_OBJECT
    Q_PROPERTY(int      maxIter     READ maxIter    WRITE setMaxIter    NOTIFY maxIterChanged)
    Q_PROPERTY(float    threshold   READ threshold  WRITE setThreshold  NOTIFY thresholdChanged)
public:
    explicit QCalibrateRobertson(QObject *parent = nullptr);
    ~QCalibrateRobertson();

    int maxIter() const;
    void setMaxIter(int maxIter);

    float threshold() const;
    void setThreshold(float threshold);

signals:
    void maxIterChanged();
    void thresholdChanged();
public slots:
    QMat* process(lv::QmlObjectList* input, QList<qreal> times);
private:
    void createCalibrateRobertson();

    int                             m_maxIter;
    float                           m_threshold;
    cv::Ptr<cv::CalibrateRobertson> m_calibrateRobertson;
};

#endif // QCALIBRATEROBERTSON_H
