/****************************************************************************
**
** Copyright (C) 2014-2016 Dinu SV.
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

#ifndef QDENSEFEATUREDETECTOR_H
#define QDENSEFEATUREDETECTOR_H

#include "qfeaturedetector.h"

class QDenseFeatureDetector : public QFeatureDetector{

    Q_OBJECT

public:
    explicit QDenseFeatureDetector(QQuickItem *parent = 0);
    virtual ~QDenseFeatureDetector();

    void initialize(const QVariantMap& settings);

};

#endif // QDENSEFEATUREDETECTOR_H
