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

#ifndef SHAREDDATASTUB_H
#define SHAREDDATASTUB_H

#include <QObject>
#include "live/shared.h"

class SharedDataTestStub : public lv::Shared{

    Q_OBJECT

public:
    explicit SharedDataTestStub(QObject *parent = 0);

    QList<int>& items(){ return m_items; }

signals:

public slots:

private:
    QList<int> m_items;
};

#endif // SHAREDDATASTUB_H
