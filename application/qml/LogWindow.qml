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

import QtQuick 2.0
import QtQuick.Window 2.1
import QtQuick.Controls 1.2
import QtQuick.Controls.Styles 1.2

Window {
    id : container
    width: 400
    height: 200
    title: "LCV Log"
    color : "#05111b"

    property string text : ""

    ScrollView {
        id: logScroll
        anchors.fill: parent
        anchors.margins: 5
        clip: true

        style: ScrollViewStyle {
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
                    color: "#091a27"
                }
            }
            decrementControl: null
            incrementControl: null
            frame: Rectangle{color: "#071723"}
            corner: Rectangle{color: "#071723"}
        }

        TextEdit{
            id : logText
            anchors.top : parent.top
            anchors.topMargin: 0
            text : container.text
            onTextChanged: {
                if ( logScroll.flickableItem.contentHeight > logScroll.height )
                    logScroll.flickableItem.contentY = logScroll.flickableItem.contentHeight - logScroll.height
            }
            textFormat: Text.RichText
            readOnly: true
            font.family: "Source Code Pro, Ubuntu Mono, Courier New, Courier"
            font.pixelSize: 12
            color : "#eee"
        }

    }
}
