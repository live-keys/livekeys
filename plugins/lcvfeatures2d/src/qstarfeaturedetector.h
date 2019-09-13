/****************************************************************************
**
** Copyright (C) 2014-2018 Dinu SV.
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

#ifndef QSTARFEATUREDETECTOR_H
#define QSTARFEATUREDETECTOR_H

#include "qfeaturedetector.h"
/// \private
class QStarFeatureDetector : public QFeatureDetector{

    Q_OBJECT

public:
    explicit QStarFeatureDetector(QQuickItem *parent = 0);
    ~QStarFeatureDetector();

    void initialize(const QVariantMap& settings);

};

#endif // QSTARFEATUREDETECTOR_H
