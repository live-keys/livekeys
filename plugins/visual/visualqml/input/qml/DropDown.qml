/****************************************************************************
**
** Copyright (C) 2022 Dinu SV.
**
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
import QtQuick.Controls 2.4
import live 1.0
import visual.shapes 1.0

ComboBox{
    id: root

    height: 25
    width: 70
    font.pixelSize: 9

    property QtObject style: DropDownStyle{}

    contentItem: Item{
        Text{
            anchors.fill: parent
            anchors.margins: 1
            anchors.leftMargin: 3
            text: root.displayText
            font: root.style.textStyle.font
            color: root.style.textStyle.color
            verticalAlignment: Text.AlignVCenter
        }
    }

    delegate: ItemDelegate {
        id: dropDownBoxDelegate
        width: root.width
        height: 20
        padding: 0
        contentItem: Rectangle{
            width: root.width + 20
            color: dropDownBoxDelegate.hovered ? root.style.backgroundColorHighlight : root.style.backgroundColor
            Text {
                text: modelData
                color: 'white'
                font: root.style.textStyle.font
                elide: Text.ElideRight
                anchors.verticalCenter: parent.verticalCenter
                anchors.left: parent.left
                anchors.leftMargin: 2
            }
        }
        highlighted: root.highlightedIndex === index
    }

    background: Rectangle {
        implicitWidth: 120
        implicitHeight: 40
        border.color: root.style.borderColor
        border.width: root.style.borderSize
        radius: 2
        color: root.style.backgroundColor
    }

    indicator: Triangle{
        x: root.width - width - 7
        y: root.topPadding + (root.availableHeight - height) / 2
        rotation: Triangle.Bottom
        width: 6
        height: 6
        color: root.style.textStyle.color
    }

    model: []

}


