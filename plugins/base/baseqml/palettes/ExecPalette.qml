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

import QtQuick 2.3
import QtQuick.Controls 1.2
import QtQuick.Controls.Styles 1.4
import editor 1.0
import live 1.0
import base 1.0
import workspace 1.0 as Workspace

CodePalette{
    id: palette

    type: "qml/Exec"

    property QtObject paletteStyle : lk ? lk.layers.workspace.extensions.editqml.paletteStyle : null

    item: Item{
        id: execBox
        width: 100
        height: 25

        property var current : null

        Workspace.TextButton{
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.left: parent.left
            anchors.leftMargin: 10
            height: 28
            width: 80
            style: palette.paletteStyle ? palette.paletteStyle.buttonStyle : defaultStyle


            text: "Run"
            onClicked: {
                execBox.current.run()
            }
        }
    }

    onInit: {
        execBox.current = value
    }

    onExtensionChanged: {
        extension.whenBinding = function(){}
    }
}
