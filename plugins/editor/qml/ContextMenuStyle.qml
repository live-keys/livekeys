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

import QtQuick 2.5
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4

MenuStyle{
    frame: Rectangle{
        color: "#030a11"
        opacity: 0.95
    }
    itemDelegate.label: Rectangle{
        width: dirLabelMenu.width
        height: dirLabelMenu.height + 6
        color: 'transparent'
        Text{
            id: dirLabelMenu
            color: styleData.enabled ? "#9babb8" : "#555"
            anchors.centerIn: parent
            text: styleData.text
            font.family: 'Open Sans, Arial, sans-serif'
            font.pixelSize: 12
            font.weight: Font.Light
        }
    }
    itemDelegate.shortcut: Rectangle{
        width: dirShortCutText.width
        height: dirShortCutText.height + 6
        color: 'transparent'
        Text{
            id: dirShortCutText
            color: "#7b8b98"
            anchors.right: parent.right
            anchors.bottom: parent.bottom
            text: styleData.shortcut
            font.family: 'Open Sans, Arial, sans-serif'
            font.pixelSize: 9
            font.weight: Font.Light
        }
    }
    itemDelegate.background: Rectangle{
        color: styleData.selected && styleData.enabled ? "#0a1a27" : "transparent"
    }
}
