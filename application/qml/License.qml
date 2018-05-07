/****************************************************************************
**
** Copyright (C) 2014-2018 Dinu SV.
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
import QtQuick.Window 2.1
import QtQuick.Controls 1.2
import QtQuick.Controls.Styles 1.2
import QtQuick.Dialogs 1.2

Rectangle{
    id: root

    visible: false

    signal close()

    color: "transparent"
    property color backgroundColor: "#010d16"
    property color titleColor: "#afafaf"
    property color containerBackgroundColor: "#081017"
    property color licenseBackgroundColor: "#010d16"
    property color licenseTextColor: "#93999e"
    property color licenseItemBackground: "#04121d"
    property color licenseItemTextColor: "#afafaf"
    property color licenseItemHighlightBackground: "#051826"
    property color licenseItemRequiredBackground: "#883120"
    property color licenseItemRequiredHighlightBackground: "#973826"
    property color licenseAcceptButtonColor: "#091925"
    property color licenseAcceptButtonHighlightColor: "#102b40"
    property color licenseAcceptButtonBorder: "#0d2434"
    property color licenseAcceptButtonTextColor: "#93999e"

    function closeBox(){
        visible = false
        close()
    }

    Keys.onEscapePressed: root.closeBox()


    MouseArea{
        anchors.fill: parent
        onClicked: mouse.accepted = true;
        onPressed: mouse.accepted = true;
        onReleased: mouse.accepted = true;
        onDoubleClicked: mouse.accepted = true;
        onPositionChanged: mouse.accepted = true;
        onPressAndHold: mouse.accepted = true;
        onWheel: wheel.accepted = true;
    }

    Rectangle{
        anchors.fill: parent
        color: "#000"
        opacity: root.visible ? 0.7 : 0
        Behavior on opacity{ NumberAnimation{ duration: 250} }
    }

    Rectangle{
        id: box
        anchors.centerIn: parent
        color: root.backgroundColor
        width: 800
        height: 550
        focus: true
        opacity: root.visible ? 1 : 0
        Behavior on opacity{ NumberAnimation{ duration: 250} }

        Keys.onEscapePressed: {
            root.visible = false
            root.cancel()
        }

        Rectangle{
            id: boxTop
            width: parent.width
            height: 35
            color: 'transparent'
            Text{
                anchors.centerIn: parent
                text: 'Manage Licenses'

                color: root.titleColor
                font.pixelSize: 12
                font.family: "Open Sans, sans-serif"
                font.weight: Font.Light
            }
            Rectangle{
                anchors.right: parent.right
                anchors.rightMargin: 10
                anchors.verticalCenter: parent.verticalCenter
                width: 30
                height: 30
                color: "transparent"
                Text{
                    anchors.centerIn: parent
                    text: "x"
                    color: root.titleColor
                    font.pixelSize: closeBoxArea.containsMouse ? 22 : 18
                    font.family: "Open Sans, sans-serif"
                    font.weight: Font.Light
                    Behavior on font.pixelSize{ NumberAnimation{ duration: 250 } }
                }
                MouseArea{
                    id: closeBoxArea
                    anchors.fill: parent
                    hoverEnabled: true
                    onClicked: root.closeBox()
                }
            }
        }

        Rectangle{
            id: boxLeft
            anchors.left: parent.left
            anchors.leftMargin: 2
            anchors.top: parent.top
            anchors.topMargin: 35
            anchors.bottom: parent.bottom
            anchors.bottomMargin: 2
            width: 230
            color: root.containerBackgroundColor

            // ListView
            ScrollView{
                anchors.fill: parent

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
                            color: "transparent"
                        }
                    }
                    decrementControl: null
                    incrementControl: null
                    frame: Rectangle{color: "transparent"}
                    corner: Rectangle{color: "transparent"}
                }

                ListView{
                    id: licenseList
                    model : livecv.settings.file('license')
                    width: parent.width
                    height: parent.height
                    clip: true

                    boundsBehavior : Flickable.StopAtBounds
                    highlightMoveDuration: 100

                    property int delegateHeight : 50

                    delegate: Rectangle{

                        property string licenseId: model.id
                        property bool isValid: model.valid

                        color: {
                            if ( model.highlight ){
                                return ListView.isCurrentItem || licenseItemArea.containsMouse ?
                                        root.licenseItemRequiredHighlightBackground : root.licenseItemRequiredBackground
                            } else {
                                return ListView.isCurrentItem || licenseItemArea.containsMouse ?
                                        root.licenseItemHighlightBackground : root.licenseItemBackground
                            }
                        }
                        width: licenseList.width
                        height: licenseList.delegateHeight
                        Text{
                            anchors.left: parent.left
                            anchors.leftMargin: 20
                            anchors.verticalCenter: parent.verticalCenter
                            text: model.alias
                            color: root.licenseItemTextColor

                            font.family: "Open Sans, sans-serif"
                            font.pixelSize: 12
                            font.weight: Font.Light
                        }
                        Image{
                            visible: model.valid
                            anchors.right: parent.right
                            anchors.rightMargin: 15
                            anchors.verticalCenter: parent.verticalCenter
                            source : "qrc:/images/license-check.png"
                        }

                        MouseArea{
                            id: licenseItemArea
                            anchors.fill: parent
                            hoverEnabled: true
                            onClicked: {
                                licenseList.currentIndex = index
                            }
                        }
                    }
                }
            }

        }
        Rectangle{
            id: boxRight
            anchors.left: boxLeft.right
            anchors.leftMargin: 2
            anchors.top: parent.top
            anchors.topMargin: 35
            anchors.right: parent.right
            anchors.rightMargin: 2
            anchors.bottom: parent.bottom
            anchors.bottomMargin: 2
            color: root.containerBackgroundColor

            ScrollView{
                anchors.fill: parent
                anchors.topMargin: 2
                anchors.leftMargin: 2
                anchors.rightMargin: 2
                anchors.bottomMargin: acceptButton.visible ? 60 : 2
                style: ScrollViewStyle {
                    transientScrollBars: false
                    handle: Item {
                        implicitWidth: 10
                        implicitHeight: 10
                        Rectangle {
                            color: "#061724"
                            anchors.fill: parent
                        }
                    }
                    scrollBarBackground: Item{
                        implicitWidth: 10
                        implicitHeight: 10
                        Rectangle{
                            anchors.fill: parent
                            color: root.licenseBackgroundColor
                        }
                    }
                    decrementControl: null
                    incrementControl: null
                    frame: Rectangle{color: root.licenseBackgroundColor}
                    corner: Rectangle{color: root.licenseBackgroundColor}
                }

                Text{
                    id: messageLabel
                    anchors.left: parent.left
                    anchors.leftMargin: 12
                    anchors.top: parent.top
                    anchors.topMargin: 12

                    text: licenseList.currentItem
                          ? livecv.settings.file('license').licenseText(licenseList.currentItem.licenseId)
                          : "No license selected."

                    width: boxRight.width - 24

                    wrapMode: Text.WordWrap

                    color: "#ccc"
                    font.pixelSize: 12
                    font.family: "Open Sans, sans-serif"
                    font.weight: Font.Light
                }

            }

            Rectangle{
                id: acceptButton
                anchors.bottom: parent.bottom
                anchors.bottomMargin: 16
                anchors.right: parent.right
                anchors.rightMargin: 15
                width: 100
                height: 35
                visible: licenseList.currentItem ?
                             licenseList.currentItem.isValid ? false : true : false
                color: acceptButtonArea.containsMouse ?
                           root.licenseAcceptButtonHighlightColor : root.licenseAcceptButtonColor
                border.width: 1
                border.color: root.licenseAcceptButtonBorder
                Text{
                    color: root.licenseAcceptButtonTextColor
                    font.pixelSize: 12
                    font.family: "Open Sans, sans-serif"
                    font.weight: Font.Light
                    text: "Accept"
                    anchors.centerIn: parent
                }
                MouseArea{
                    id: acceptButtonArea
                    anchors.fill: parent
                    hoverEnabled: true
                    onClicked: livecv.settings.file('license').acceptLicense(licenseList.currentItem.licenseId)
                }
            }
        }

    }
}
