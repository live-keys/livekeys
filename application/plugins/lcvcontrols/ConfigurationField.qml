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

import QtQuick 2.3

Rectangle {
    id: root

    width: 500
    height: 30

    property string key : "key"
    property string label : "Label"

    property alias editor : editorContainer.children

    signal dataChanged(string data)

    Rectangle{
        id: labelContainer
        width: 200
        height: parent.height

        Text{
            id : label

            anchors.right: parent.right
            anchors.rightMargin: 10
            anchors.verticalCenter : parent.verticalCenter

            text: root.label
            font.pixelSize : 13
            font.family : "Courier New, Courier"
            font.weight: Font.Normal
        }
    }


    Rectangle{
        id: editorContainer
        color: "#ff0000"
        height: parent.height
        width: parent.width - labelContainer.width
        anchors.right: parent.right

    }
}

