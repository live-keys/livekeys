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
import QtQuick.Controls 1.2
import QtQuick.Controls.Styles 1.4
import editor 1.0
import base 1.0
import visual.input 1.0 as Input
import visual.shapes 1.0 as Shapes
import workspace.icons 1.0 as Icons

CodePalette{
    id: palette

    type : "qml/base#Arrange"

    property QtObject theme : lk.layers.workspace.themes.current

    item: Item{
        id: arrangeItem
        width: listView.width
        height: listView.height + (arrangeItem.isDirty ? 55 : 25)

        property QtObject arrange: null

        function addEntry(from, to){
            var model = listView.model
            for ( var i = 0; i < model.length; ++i ){
                if ( model[i].from === from )
                    return
            }

            model.push({from: from, to: to})
            listView.model = model
        }

        property string type: ''
        property bool isDirty: false

        signal cancel()
        signal ready(var data)
        onReady:{
            palette.valueSelected(data)
            arrangeItem.isDirty = false
        }

        Input.SelectableListView{
            id: listView
            anchors.top: parent.top
            anchors.topMargin: 0
            width: 250

            model: []

            delegate: Component{
                Item{
                    id: listViewDelegate
                    height: 25
                    width: listView.width

                    property Item paletteItem: palette.item

                    Rectangle{
                        width : 100
                        height : 24
                        color : palette.theme.colorScheme.middleground
                        Input.Label{
                            id: label
                            anchors.left: parent.left
                            anchors.leftMargin: 10
                            anchors.verticalCenter: parent.verticalCenter
                            textStyle: listView.style.labelStyle
                            text: modelData.from
                        }
                        Shapes.Triangle{
                            anchors.left: parent.left
                            anchors.leftMargin: 110
                            anchors.top: parent.top
                            anchors.topMargin: 7
                            rotation: Shapes.Triangle.Right
                            color: palette.theme.colorScheme.foregroundFaded
                            width: 8
                            height: 8
                        }
                    }

                    Rectangle{
                        anchors.left: parent.left
                        anchors.leftMargin: 130
                        width : 120
                        height : 24
                        color : palette.theme.colorScheme.middleground

                        Input.Label{
                            id: labelTo
                            anchors.left: parent.left
                            anchors.leftMargin: 10
                            anchors.verticalCenter: parent.verticalCenter
                            textStyle: listView.style.labelStyle
                            text: modelData.to
                        }
                    }

                    Image{
                        anchors.right: parent.right
                        anchors.rightMargin: 15
                        anchors.top: parent.top
                        anchors.topMargin: 7
                        source : "qrc:/images/palette-erase-object.png"
                        MouseArea {
                            anchors.fill: parent
                            onClicked: {
                                var idx = index
                                var model = listView.model
                                model.splice(idx, 1)
                                listView.model = model

                                listViewDelegate.paletteItem.isDirty = true
                            }
                        }
                    }
                }
            }
        }


        Input.Label{
            id: label2
            anchors.bottom: parent.bottom
            anchors.bottomMargin: arrangeItem.isDirty ? 32 : 4
            anchors.left: parent.left
            anchors.leftMargin: 10
            textStyle: palette.theme.inputLabelStyle.textStyle
            text: listView.model.length + (listView.model.length === 1 ? " entry" : " entries")
        }

        Input.Button{
            id: showAddEntry
            anchors.bottom: parent.bottom
            anchors.bottomMargin: arrangeItem.isDirty ? 28 : 0
            anchors.right: parent.right
            anchors.rightMargin: 10
            visible: !addEntry.visible
            width: 25
            height: 25
            content: Rectangle{
                width: 25
                height: 25
                color: showAddImportsButtonArea.containsMouse
                    ? palette.theme.colorScheme.middlegroundOverlayBorder
                    : palette.theme.colorScheme.middlegroundOverlay
                radius: 15

                Icons.PlusIcon{
                    anchors.centerIn: parent
                    width: 8
                    height: 8
                    strokeWidth: 1
                    color: palette.theme.colorScheme.foregroundFaded
                }

                MouseArea{
                    id : showAddImportsButtonArea
                    hoverEnabled: true
                    anchors.fill: parent
                    onClicked: {
                        addEntry.visible = true
                        fromInput.forceFocus()
                    }
                }
            }
        }

        Item{
            id: addEntry
            height: visible ? 30 : 0
            width: parent.width
            visible: false
            anchors.bottom: parent.bottom
            anchors.bottomMargin: arrangeItem.isDirty ? 28 : 0

            Item{
                id: addImport
                anchors.left: parent.left
                anchors.leftMargin: 0
                width: parent.width - 55
                height : visible ? 25 : 0

                Input.InputBox{
                    id: fromInput
                    anchors.left: parent.left
                    anchors.leftMargin: 3
                    margins: 4

                    width: parent.width / 2 - 15
                    height: 25
                    textHint: ''

                    nextFocusItem: toInput
                    prevFocusItem: toInput

                    onKeyPressed: {
                        if ( event.key === Qt.Key_Return ){
                            var from = fromInput.text
                            var to = toInput.text

                            if ( from !== '' && to !== '' ){
                                fromInput.text = ''
                                fromInput.focus = false
                                toInput.text = ''
                                toInput.focus = false
                                addEntry.visible = false

                                arrangeItem.addEntry(from, to)
                                arrangeItem.isDirty = true
                            }
                        }
                    }

                    style: theme.inputStyle
                }


                Shapes.Triangle{
                    anchors.left: parent.left
                    anchors.leftMargin: parent.width / 2 - 3
                    anchors.top: parent.top
                    anchors.topMargin: 7
                    rotation: Shapes.Triangle.Right
                    color: palette.theme.colorScheme.foregroundFaded
                    width: 8
                    height: 8
                }


                Input.InputBox{
                    id: toInput
                    anchors.left: parent.left
                    anchors.leftMargin: parent.width / 2 + 12
                    margins: 4

                    width: parent.width / 2 - 15
                    height: 25
                    textHint: ''
                    nextFocusItem: fromInput
                    prevFocusItem: fromInput

                    onKeyPressed: {
                        if ( event.key === Qt.Key_Return ){
                            var from = fromInput.text
                            var to = toInput.text

                            if ( from !== '' && to !== '' ){
                                fromInput.text = ''
                                fromInput.focus = false
                                toInput.text = ''
                                toInput.focus = false
                                addEntry.visible = false

                                arrangeItem.addEntry(from, to)
                                arrangeItem.isDirty = true
                            }

                        }
                    }

                    style: palette.theme.inputStyle
                }
            }

            Input.Button{
                id: commitButton
                anchors.right: parent.right
                anchors.rightMargin: 25
                width: 25
                height: 25
                content: palette.theme.buttons.apply
                onClicked: {
                    var from = fromInput.text
                    var to = toInput.text

                    fromInput.text = ''
                    fromInput.focus = false
                    toInput.text = ''
                    toInput.focus = false
                    addEntry.visible = false

                    arrangeItem.addEntry(from, to)
                    arrangeItem.isDirty = true
                }
            }
            Input.Button{
                id: cancelButton
                anchors.right: parent.right
                width: 25
                height: 25
                content: palette.theme.buttons.cancel
                onClicked: {
                    fromInput.text = ''
                    fromInput.focus = false
                    toInput.text = ''
                    toInput.focus = false
                    addEntry.visible = false
                }
            }

        }


        Input.TextButton{
            id: updateButton
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.bottomMargin: 0
            anchors.bottom: parent.bottom
            height: 25
            width: 70
            visible: arrangeItem.isDirty
            style: palette.theme.formButtonStyle

            text: "Done"
            onClicked: {
                arrangeItem.ready(listView.model)
            }
        }
    }

    property var valueSelected: function(value){
        var convert = {}
        for ( var i = 0; i < value.length; ++i ){
            convert[value[i].from] = value[i].to
        }

        arrangeItem.arrange.map = convert
        editFragment.writeProperties({
            map: convert
        })
    }

    onValueFromBindingChanged: {
        arrangeItem.arrange = value
        if ( !value )
            return

        var valueMap = value.map

        var model = []
        for (var key in valueMap) {
            if (valueMap.hasOwnProperty(key)) {
                mode.push({ from: key, to: valueMap[key]})
            }
        }
        listView.model = model
    }
    onInit: {
        arrangeItem.arrange = value
        if ( !value )
            return

        var valueMap = value.map

        var model = []
        for (var key in valueMap) {
            if (valueMap.hasOwnProperty(key)) {
                model.push({ from: key, to: valueMap[key]})
            }
        }
        listView.model = model

        editFragment.whenBinding = function(){
            editFragment.writeProperties({
                'map' : palette.value.map
            })
        }
    }
}
