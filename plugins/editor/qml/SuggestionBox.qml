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
import QtQuick.Controls 2.2
import base 1.0
import editor.private 1.0
import visual.input 1.0 as Input

Rectangle{
    id: root

    width: 250
    height: 280
    color: "#03070a"
    opacity: 0.95

    property color selectionColor: "#091927"
    property alias suggestionCount : pluginList.count
    property alias model : pluginList.model
    property string fontFamily: 'Courier New, Courier'
    property int fontSize: 12
    property int smallFontSize: 9

    function __initialize(font){
        root.fontFamily = font.family
        root.fontSize = font.pixelSize
        root.smallFontSize = font.pixelSize - 2
    }

    function getCompletion(){
        if ( pluginList.currentItem ){
            return pluginList.currentItem.completion
        }
        return ""
    }

    function getDocumentation(){
        if ( pluginList.currentItem ){
            return pluginList.currentItem.documentation
        }
        return ""
    }

    function highlightNext(){
        if ( pluginList.currentIndex + 1 <  pluginList.count ){
            pluginList.currentIndex++;
        } else {
            pluginList.currentIndex = 0;
        }
    }
    function highlightPrev(){
        if ( pluginList.currentIndex > 0 ){
            pluginList.currentIndex--;
        } else {
            pluginList.currentIndex = pluginList.count - 1;
        }
    }

    function highlightNextPage(){
        var noItems = Math.floor(pluginList.height / 25)
        if ( pluginList.currentIndex + noItems < pluginList.count ){
            pluginList.currentIndex += noItems;
        } else {
            pluginList.currentIndex = pluginList.count - 1;
        }
    }
    function highlightPrevPage(){
        var noItems = Math.floor(pluginList.height / 25)
        if ( pluginList.currentIndex - noItems >= 0 ){
            pluginList.currentIndex -= noItems;
        } else {
            pluginList.currentIndex = 0;
        }
    }

    ScrollView{
        id: scrollView
        anchors.top : parent.top

        height : root.height
        width: root.width

        ScrollBar.vertical.policy: ScrollBar.AlwaysOn
        ScrollBar.vertical.contentItem: Input.ScrollbarHandle{
            color: "#1f2227"
            visible: scrollView.contentHeight > scrollView.height
        }
        ScrollBar.horizontal.policy: ScrollBar.AlwaysOn
        ScrollBar.horizontal.contentItem: Input.ScrollbarHandle{
            color: "#1f2227"
            visible: scrollView.contentWidth > scrollView.width
        }

        ListView{
            id : pluginList
            anchors.fill: parent
            anchors.rightMargin: 2
            anchors.bottomMargin: 5
            anchors.topMargin: 0
            visible: true
            opacity: root.opacity

            currentIndex: 0
            onCountChanged: currentIndex = 0

            boundsBehavior : Flickable.StopAtBounds
            highlightMoveDuration: 100

            delegate: Component{

                Rectangle{
                    property string completion: model.completion
                    property string documentation: model.documentation

                    width : pluginList.width
                    height : 25
                    color : ListView.isCurrentItem ? root.selectionColor : "transparent"
                    Text{
                        id: label
                        anchors.left: parent.left
                        anchors.leftMargin: 10
                        anchors.verticalCenter: parent.verticalCenter

                        font.family: root.fontFamily
                        font.pixelSize: root.fontSize

                        color: "#fafafa"
                        text: model.label
                    }
                    Text{
                        id: info
                        anchors.left: label.right
                        anchors.leftMargin: 10
                        anchors.verticalCenter: parent.verticalCenter

                        font.family: root.fontFamily
                        font.pixelSize: root.smallFontSize

                        color: "#173e5f"
                        text: model.info
                    }
                    Text{
                        id: categoryt
                        anchors.right: parent.right
                        anchors.rightMargin: 10
                        anchors.verticalCenter: parent.verticalCenter

                        font.family: root.fontFamily
                        font.pixelSize: root.smallFontSize - 1

                        color: "#bc900c"
                        text: model.category
                    }

                    MouseArea{
                        anchors.fill: parent
                    }
                }
            }

        }
    }
}
