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
import QtQuick.Controls.Styles 1.2
import base 1.0
import live 1.0
import editor.private 1.0
import workspace 1.0 as Workspace

Rectangle{
    id: root

    width: 380 + (mode & AddQmlBox.DisplayMode.WithFunctions ? 100: 0)
    height: 280
    color: root.theme.colorScheme.background
    opacity: 0.95
    objectName: "addQmlBox"

    enum DisplayMode {
        ObjectsOnly = 0,
        Default = 1,
        WithFunctions = 2,
        NoObjects = 4
    }

    enum AddPropertyMode {
        Default = 0,
        AddAsReadOnly = 1,
        AddAsObject = 2
    }

    property QtObject theme: lk.layers.workspace.themes.current

    property QtObject addContainer : null

    property string fontFamily: root.theme.inputLabelStyle.textStyle.font.family
    property int fontSize: root.theme.inputLabelStyle.textStyle.font.pixelSize

    property int smallFontSize: 9
    property var codeQmlHandler: null

    property var mode: AddQmlBox.DisplayMode.Default

    border.color: root.theme.colorScheme.middlegroundOverlayDominant
    border.width: 1

    onModeChanged: {
        if (mode === AddQmlBox.DisplayMode.ObjectsOnly)
            activeIndex = 2
    }

    property int activeIndex : 0
    property bool idChecked: false
    onActiveIndexChanged: {
        searchInput.text = ''
        root.addContainer.model.setFilter('')
        if (activeIndex === 2) idChecked = true
        root.addContainer.model.setCategoryFilter(activeIndex)
        root.addContainer.model.setImportFilter('')
        root.addContainer.model.setTypeFilter('')
        categoryList.currentIndex = 0
        listView.currentIndex = 0

    }

    function assignFocus(){
        searchInput.forceFocus()
    }

    property var cancel: function(){ }
    property var accept : function(type, data, mode){ }

    function getCompletion(){
        if ( listView.currentItem ){
            return listView.currentItem.completion
        }
        return ""
    }

    function highlightNext(){ listView.highlightNext() }
    function highlightPrev(){ listView.highlightPrev() }

    function highlightNextPage(){ listView.highlightNextPage() }
    function highlightPrevPage(){ listView.highlightPrevPage() }

    Item {
        id: header
        height: title.height + buttonsContainer.height
        anchors.top: parent.top

        Workspace.Label{
            id: title
            anchors.top: parent.top
            anchors.topMargin: 5
            anchors.left: parent.left
            anchors.leftMargin: 5
            height: 25
            width: parent.width
            textStyle: root.theme.inputLabelStyle.textStyle

            text: {
                if (!addContainer) return ""
                var result;
                switch (activeIndex) {
                    case 0: result = "All"; break;
                    case 1: result = "Properties"; break;
                    case 2: result = "Items"; break;
                    case 3: result = "Events"; break;
                    case 4: result = "Functions"; break;
                }

                result = result + " for "  + addContainer.objectType
            }
        }

        Row {
            id: buttonsContainer
            anchors.top: title.bottom
            height: 30
            spacing: 2

            Workspace.TextButton{
                visible: mode !== AddQmlBox.DisplayMode.ObjectsOnly
                text: 'All'
                height: 22
                width: 70
                style: root.theme.formButtonStyle
                color: {
                    activeIndex === 0 ? style.backgroundHoverColor : containsMouse ? style.backgroundHoverColor : style.backgroundColor
                }
                onClicked : {
                    root.activeIndex = 0
                }
            }

            Workspace.TextButton{
                visible: mode !== AddQmlBox.DisplayMode.ObjectsOnly
                text: 'Property'
                height: 22
                width: 70
                style: root.theme.formButtonStyle
                color: {
                    activeIndex === 1 ? style.backgroundHoverColor : containsMouse ? style.backgroundHoverColor : style.backgroundColor
                }
                onClicked : {
                    root.activeIndex = 1
                }
            }

            Workspace.TextButton{
                text: 'Object'
                height: 22
                width: 70
                visible: !(mode & AddQmlBox.DisplayMode.NoObjects)

                style: root.theme.formButtonStyle
                color: {
                    activeIndex === 2 ? style.backgroundHoverColor : containsMouse ? style.backgroundHoverColor : style.backgroundColor
                }

                onClicked : {
                    root.activeIndex = 2
                }
            }

            Workspace.TextButton{
                visible: mode !== AddQmlBox.DisplayMode.ObjectsOnly
                text: 'Event'
                height: 22
                width: 70
                style: root.theme.formButtonStyle
                color: {
                    activeIndex === 3 ? style.backgroundHoverColor : containsMouse ? style.backgroundHoverColor : style.backgroundColor
                }

                onClicked : {
                    root.activeIndex = 3
                }
            }

            Workspace.TextButton{
                visible: mode & AddQmlBox.DisplayMode.WithFunctions
                text: 'Function'
                height: 22
                width: 70
                style: root.theme.formButtonStyle
                color: {
                    activeIndex === 4 ? style.backgroundHoverColor : containsMouse ? style.backgroundHoverColor : style.backgroundColor
                }

                onClicked : {
                    root.activeIndex = 4
                }
            }
        }
    }


    Item {
        id: idInputItem
        visible: activeIndex === 2
        height: 30
        width: parent.width
        anchors.top: parent.top
        anchors.topMargin: header.height

        Rectangle {
            x: 15
            y: 5
            width: 16
            height: 16
            border.width: 2
            border.color: root.theme.colorScheme.middlegroundOverlayDominant
            color: "transparent"

            Rectangle {
                color: root.theme.colorScheme.middlegroundOverlayDominant
                width: 8
                height: 8
                x: 4
                y: 4
                visible: idChecked
            }
            MouseArea {
                anchors.fill: parent
                onClicked: {
                    idChecked = !idChecked
                }
            }
        }

        Text {
            x: 45
            y: 3
            text: "Id"
            color : "#efefef"
            font.family: "Open Sans, sans-serif"
            font.pixelSize: 14
            font.weight: Font.Normal
        }

        Rectangle {
            x: 60
            y: 5
            width: parent.width - 70
            height: 18
            anchors.right: parent.right
            anchors.rightMargin: 0
            color: "transparent"

            Workspace.InputBox {

                id : idInput

                property bool userInput: false
                anchors.verticalCenter: parent.verticalCenter
                anchors.left: parent.left
                anchors.leftMargin: 8
                anchors.right: parent.right
                anchors.rightMargin: 1
                style: root.theme.inputStyle

                onActiveFocusLost: {
                    if (!searchInput.inputActiveFocus && !idInput.inputActiveFocus)
                        root.cancel()
                }

                width: (parent.width > implicitWidth ? parent.width : implicitWidth)

                text: {
                    if (!listView || !listView.currentItem) return text

                    if (!userInput)
                    {
                        var id = listView.currentItem.code
                        var result = id[0].toLowerCase() + id.substring(1)
                        var index = 1
                        if (codeQmlHandler)
                        {

                            var origResult = result
                            var docIds = codeQmlHandler.getDocumentIds()

                            while (true)
                            {
                                var found = false
                                for (var i=0; i<docIds.length;++i)
                                    if (docIds[i] === result)
                                    {
                                        found = true
                                        break
                                    }
                                if (!found) break
                                index++
                                result = origResult + index
                            }

                            return result
                        }

                        return result
                    }

                    return text
                }

                MouseArea{
                    anchors.fill: parent
                    acceptedButtons: Qt.NoButton
                    cursorShape: Qt.IBeamCursor
                }

                onKeyPressed: {
                    userInput = true
                    if ( event.key === Qt.Key_Enter || event.key === Qt.Key_Return ){
                        root.acceptSelection()
                    }
                }
            }
        }

    }

    Rectangle{
        id: searchInputBox
        anchors.top: parent.top
        anchors.topMargin: idInputItem && idInputItem.visible? header.height + 30 : header.height
        anchors.left: parent.left
        anchors.leftMargin: 1
        width: parent.width - 1

        color: "transparent"
        height: 28

        Workspace.InputBox {
            id : searchInput
            anchors.verticalCenter: parent.verticalCenter
            anchors.left: parent.left
            anchors.leftMargin: 1
            anchors.right: parent.right
            anchors.rightMargin: 1

            width: parent.width > implicitWidth ? parent.width : implicitWidth
            style: root.theme.inputStyle

            onActiveFocusLost: {
                if (!searchInput.inputActiveFocus && !idInput.inputActiveFocus)
                    root.cancel()
            }

            text: ""
            onTextChanged: {
                root.addContainer.model.setFilter(text)
            }

            MouseArea{
                anchors.fill: parent
                acceptedButtons: Qt.NoButton
                cursorShape: Qt.IBeamCursor
            }

            Keys.onPressed: {
                if ( event.key === Qt.Key_Down ){
                    root.highlightNext()
                    event.accepted = true
                } else if ( event.key === Qt.Key_Up ){
                    root.highlightPrev()
                    event.accepted = true
                } else if ( event.key === Qt.Key_PageDown ){
                    root.highlightNextPage()
                    event.accepted = true
                } else if ( event.key === Qt.Key_PageUp ){
                    root.highlightPrevPage()
                    event.accepted = true
                }
            }
            Keys.onReturnPressed: {
                acceptSelection()
            }
            Keys.onEscapePressed: {
                root.cancel()
            }
        }
    }

    function acceptSelection(){
        if (!listView.currentItem) {
            root.cancel()
            return
        }

        var selector = root.activeIndex === 0 ? listView.currentItem.category : root.activeIndex
        if (root.activeIndex === 0) idChecked = true // add id by default

        var type = listView.currentItem.type
        var code = listView.currentItem.code
        var importSpace = listView.currentItem.importSpace

        var mode = AddQmlBox.AddPropertyMode.Default
        if (listView.currentItem.isGroup && listView.currentItem.isWritable){
            if (listView.currentItem.itemChecked){
                mode = AddQmlBox.AddPropertyMode.AddAsReadOnly
            } else {
                mode = AddQmlBox.AddPropertyMode.AddAsObject
            }
        }

        if (selector === 2){
            var result = code
            if (idChecked && idInput.text !== "") result = result + "#" + idInput.text
            root.activeIndex = 2
            root.accept(importSpace, result, mode)
        } else {
            root.activeIndex = selector
            root.accept(type, code, mode)
        }
    }

    Item{
        id: container
        anchors.fill: parent
        anchors.topMargin: idInputItem && idInputItem.visible ? header.height + 60 : header.height + 30
        anchors.leftMargin: 1
        anchors.rightMargin: 1
        anchors.bottomMargin: 1

        Workspace.SelectableListView {
            id: categoryList
            anchors.top : parent.top
            anchors.left: parent.left

            height : root.height - container.anchors.topMargin - 1
            width: root.width / 2

            model: {
                if (!root.addContainer) return null
                var types = root.addContainer.model.types()
                var importSpaces = root.addContainer.model.importSpaces()

                if (activeIndex === 0){
                    for (var i=1; i < types.length; ++i)
                        types[i] = "c  " + types[i]

                    importSpaces.splice(0,1)
                    for (var i=0; i < importSpaces.length; ++i)
                        importSpaces[i] = "{  " + importSpaces[i]

                    return types.concat(importSpaces)
                }
                return (activeIndex === 2 ? importSpaces : types)
            }
            delegate: Component{

                Rectangle{
                    width : categoryList.width

                    height : 25
                    color : ListView.isCurrentItem ? root.theme.selectableListView.selectionBackgroundColor : "transparent"
                    Workspace.Label{
                        id: label
                        anchors.left: parent.left
                        anchors.leftMargin: 10
                        anchors.verticalCenter: parent.verticalCenter
                        textStyle: root.theme.inputLabelStyle.textStyle
                        text: modelData
                    }

                    MouseArea{
                        anchors.fill: parent
                        onClicked: {
                            categoryList.currentIndex = index
                            if (root.activeIndex === 0){
                                var selector = modelData[0]
                                var text = modelData.substring(3)

                                if (selector ===  'A') { // All
                                    root.addContainer.model.setImportFilter('')
                                    root.addContainer.model.setTypeFilter('')
                                } else if (selector === '{'){
                                    root.addContainer.model.setImportFilter(text)
                                    root.addContainer.model.setTypeFilter('')
                                } else {
                                    root.addContainer.model.setImportFilter('')
                                    root.addContainer.model.setTypeFilter(text)
                                }

                                return
                            }
                            if ( modelData === 'All' ) {
                                if (root.activeIndex === 2){
                                    root.addContainer.model.setImportFilter('')
                                } else {
                                    root.addContainer.model.setTypeFilter('')
                                }

                            }
                            else {
                                if (root.activeIndex === 2) {
                                    root.addContainer.model.setImportFilter(modelData)
                                } else {
                                    root.addContainer.model.setTypeFilter(modelData)
                                }

                            }
                        }
                    }
                }
            }
        }

        Workspace.SelectableListView {
            id: listView
            anchors.top : parent.top
            anchors.right: parent.right

            height : root.height - container.anchors.topMargin - 1
            width: root.width / 2

            model: root.addContainer ? root.addContainer.model : null

            delegate: Component{
                Rectangle{
                    id: modelItem
                    property string objectType : model.objectType
                    property string type : model.type
                    property string code: model.code
                    property int category: model.category
                    property bool isGroup: model.isGroup
                    property bool isWritable: model.isWritable
                    property bool itemChecked: false

                    width : listView.width
                    height : 25
                    color : ListView.isCurrentItem ? root.theme.selectableListView.selectionBackgroundColor : "transparent"
                    Text{
                        id: label
                        anchors.left: parent.left
                        anchors.leftMargin: 10
                        anchors.verticalCenter: parent.verticalCenter

                        font.family: root.fontFamily
                        font.pixelSize: root.fontSize
                        font.weight: Font.Normal

                        color: "#fafafa"
                        text: model.label
                    }

                    Item {
                        id: check
                        height: parent.height
                        width: height
                        visible: model.isGroup && model.isWritable

                        Rectangle {
                            width: 16
                            height: 16
                            anchors.centerIn: parent
                            border.width: 2
                            border.color: root.theme.colorScheme.middlegroundOverlayDominant
                            color: "transparent"

                            Rectangle {
                                color: root.theme.colorScheme.middlegroundOverlayDominant
                                width: 8
                                height: 8
                                x: 4
                                y: 4
                                visible: modelItem.itemChecked
                            }
                            MouseArea {
                                anchors.fill: parent
                                onClicked: {
                                    modelItem.itemChecked = !modelItem.itemChecked
                                }
                            }
                        }
                        anchors.right: parent.right
                        anchors.rightMargin: 15
                    }

                    MouseArea{
                        anchors.fill: parent
                        anchors.rightMargin: check.visible ? check.width + 15 : 15
                        onClicked: {
                            listView.currentIndex = index
                        }
                        onDoubleClicked: {
                            root.acceptSelection()
                        }
                    }
                }

            }
        }
    }

    Keys.onEscapePressed: {
        root.cancel()
    }

    MouseArea {
        id: mouseArea
        anchors.fill: parent
        z: -1
    }

}
