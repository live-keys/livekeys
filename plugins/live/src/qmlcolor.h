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

#ifndef QMLCOLOR_H
#define QMLCOLOR_H

#include <QObject>
#include <QColor>

namespace lv {

class QmlColor : public QObject{

    Q_OBJECT

public:
    explicit QmlColor(QObject *parent = nullptr);

public slots:
    QColor RgbToHsv(QColor rgb);
    QString toHex(QColor rgb);
};

}

#endif // QMLCOLOR_H
