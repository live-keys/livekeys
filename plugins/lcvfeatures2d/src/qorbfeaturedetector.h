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

#ifndef QORBFEATUREDETECTOR_H
#define QORBFEATUREDETECTOR_H

#include "qfeaturedetector.h"
/// \private
class QOrbFeatureDetector : public QFeatureDetector{

    Q_OBJECT
    Q_ENUMS(ScoreType)

public:
    enum ScoreType{
        kBytes       = 32,
        HARRIS_SCORE = 0,
        FAST_SCORE   = 1
    };

public:
    explicit QOrbFeatureDetector(QQuickItem *parent = 0);
    virtual ~QOrbFeatureDetector();

public slots:
    void initialize(const QVariantMap& settings);

};

#endif // QORBFEATUREDETECTOR_H
