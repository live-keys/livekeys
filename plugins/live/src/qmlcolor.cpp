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

#include "qmlcolor.h"
#include <QColor>
#include <QDebug>

namespace lv {

QmlColor::QmlColor(QObject *parent) : QObject(parent)
{

}

QColor QmlColor::RgbToHsv(QColor rgb)
{
    return rgb.toHsv();
}

QString QmlColor::toHex(QColor rgb){
    return rgb.name(QColor::HexRgb);
}

}
