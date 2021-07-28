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

#ifndef QCALIBRATEDEBEVEC_H
#define QCALIBRATEDEBEVEC_H

#include <QObject>
#include <QQmlParserStatus>
#include "opencv2/photo.hpp"
#include "live/qmlobjectlist.h"
#include "qmat.h"

class QCalibrateDebevec : public QObject{

    Q_OBJECT
    Q_PROPERTY(int      samples READ samples WRITE setSamples NOTIFY samplesChanged)
    Q_PROPERTY(float    lambda  READ lambda  WRITE setLambda  NOTIFY lambdaChanged)
    Q_PROPERTY(bool     random  READ random  WRITE setRandom  NOTIFY randomChanged)
public:
    explicit QCalibrateDebevec(QObject *parent = nullptr);
    ~QCalibrateDebevec();

    int samples() const;
    void setSamples(int samples);

    float lambda() const;
    void setLambda(float lambda);

    bool random() const;
    void setRandom(bool random);

signals:
    void samplesChanged();
    void lambdaChanged();
    void randomChanged();
public slots:
    QMat* process(lv::QmlObjectList* input, QList<qreal> times);
private:
    void createCalibrateDebevec();

    int                             m_samples;
    float                           m_lambda;
    bool                            m_random;
    cv::Ptr<cv::CalibrateDebevec>   m_calibrateDebevec;
};

#endif // QCALIBRATEDEBEVEC_H
