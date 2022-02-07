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

#ifndef LVEVENTRELAY_H
#define LVEVENTRELAY_H

#include <QObject>

namespace lv{

class EventRelay : public QObject{

    Q_OBJECT

public:
    explicit EventRelay(QObject *parent = nullptr);

signals:

public slots:
    void relayKeyEvent(QObject* object, int key, int modifiers);

};

}// namespace

#endif // LVEVENTRELAY_H
