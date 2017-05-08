/****************************************************************************
**
** Copyright (C) 2014-2017 Dinu SV.
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

import QtQuick 2.3
import QtQuick.Controls 1.2
import QtQuick.Controls.Styles 1.2

ScrollViewStyle {
    transientScrollBars: false
    handle: Item {
        implicitWidth: 10
        implicitHeight: 10
        Rectangle {
            color: "#0b1f2e"
            anchors.fill: parent
        }
    }
    scrollBarBackground: Item{
        implicitWidth: 10
        implicitHeight: 10
        Rectangle{
            anchors.fill: parent
            color: "#091823"
        }
    }
    decrementControl: null
    incrementControl: null
    frame: Rectangle{color: "transparent"}
    corner: Rectangle{color: "#091823"}
}
