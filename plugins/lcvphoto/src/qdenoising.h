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

#ifndef QDENOISING_H
#define QDENOISING_H

#include <QObject>
#include "qmat.h"

/// \private
class QDenoising : public QObject{

    Q_OBJECT

public:
    Q_INVOKABLE QDenoising(QObject *parent = nullptr);

public slots:
    QObject *fastNlMeans(
        QObject *input,
        float h = 3,
        int templateWindowSize = 7,
        int searchWindowSize = 21
    );

    QObject *fastNlMeansColored(
        QObject* input,
        float h = 3,
        int templateWindowSize = 7,
        int searchWindowSize = 21
    );

    QMat* denoiseTVL1(const QVariantList& obs, double lambda=1.0, int iters=30);

};

#endif // QDENOISING_H
