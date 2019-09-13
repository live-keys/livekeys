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

#ifndef QBRUTEFORCEMATCHER_H
#define QBRUTEFORCEMATCHER_H

#include "qdescriptormatcher.h"

/// \private
class QBruteForceMatcher : public QDescriptorMatcher{

    Q_OBJECT
    Q_ENUMS(NormType)

public:
    enum NormType{
        NORM_L1 = 2,
        NORM_L2 = 4,
        NORM_HAMMING = 6,
        NORM_HAMMING2 = 7
    };

public:
    QBruteForceMatcher(QQuickItem* parent = 0);
    virtual ~QBruteForceMatcher();

protected:
    virtual void initialize(const QVariantMap& params);
};

#endif // QBRUTEFORCEMATCHER_H
