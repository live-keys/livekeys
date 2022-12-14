import QtQuick 2.3
import QtQuick.Controls 2.2
import visual.shapes 1.0
import visual.input 1.0 as Input
import workspace 1.0
import editor 1.0
import editor.private 1.0

Item{
    id: root
    width: 50
    height: 30

    property Component runLiveTriggerIcon: Image{ source: "qrc:/images/top-icon-mode-live.png" }
    property Component runSaveTriggerIcon: Image{ source: "qrc:/images/top-icon-mode-onsave.png" }
    property Component runDisabledTriggerIcon: Image{ source: "qrc:/images/top-icon-mode-disabled.png" }

    property QtObject popupMenuStyle: PopupMenuStyle{}

    property int trigger: Document.RunOnChange

    signal triggerSelected(int trigger)

    Loader{
        anchors.verticalCenter: parent.verticalCenter
        anchors.left: parent.left
        anchors.leftMargin: 3

        sourceComponent: root.trigger === Document.RunOnChange
            ? root.runLiveTriggerIcon
            : root.trigger === Document.RunOnSave
                ? root.runSaveTriggerIcon
                : root.runDisabledTriggerIcon
    }

    Triangle{
        anchors.right: parent.right
        anchors.rightMargin: 7
        anchors.verticalCenter: parent.verticalCenter
        width: 9
        height: 5
        color: openStatesDropdown.containsMouse ? "#9b6804" : "#bcbdc1"
        rotation: Triangle.Bottom
    }


    MouseArea{
        id : openStatesDropdown
        anchors.fill: parent
        hoverEnabled: true
        onClicked: {
            if ( !triggerTypeList.visible )
                triggerTypeList.open()
            else
                triggerTypeList.close()
        }
    }

    Popup{
        id: triggerTypeList
        y: 30
        modal: false
        focus: true
        padding: 0
        closePolicy: Popup.CloseOnEscape | Popup.CloseOnPressOutsideParent
        width: 120
        height: 92

        contentItem: Rectangle{
            color: root.popupMenuStyle.backgroundColor
            border.width: root.popupMenuStyle.borderWidth
            border.color: root.popupMenuStyle.borderColor
            radius: root.popupMenuStyle.radius
            Column{
                width: parent.width - 2
                anchors.left: parent.left
                anchors.leftMargin: 1
                anchors.top: parent.top
                anchors.topMargin: 1

                Rectangle{
                    width: parent.width
                    height: 30
                    color: liveSelectArea.containsMouse ? root.popupMenuStyle.highlightBackgroundColor : 'transparent'

                    Input.Label{
                        text: qsTr("Live")
                        textStyle: liveSelectArea.containsMouse ? root.popupMenuStyle.highlightTextStyle : root.popupMenuStyle.textStyle
                        anchors.left: parent.left
                        anchors.leftMargin: 10
                        anchors.verticalCenter: parent.verticalCenter
                    }
                    Loader{
                        anchors.verticalCenter: parent.verticalCenter
                        anchors.right: parent.right
                        anchors.rightMargin: 10
                        sourceComponent: root.runLiveTriggerIcon
                    }
                    MouseArea{
                        id: liveSelectArea
                        anchors.fill: parent
                        hoverEnabled: true
                        onClicked: {
                            triggerTypeList.close()
                            root.triggerSelected(Document.RunOnChange)
                        }
                    }
                }

                Rectangle{
                    width: parent.width
                    height: 30
                    color: saveeSelectArea.containsMouse ? root.popupMenuStyle.highlightBackgroundColor : 'transparent'

                    Input.Label{
                        text: qsTr("On Save")
                        textStyle: saveeSelectArea.containsMouse ? root.popupMenuStyle.highlightTextStyle : root.popupMenuStyle.textStyle
                        anchors.left: parent.left
                        anchors.leftMargin: 10
                        anchors.verticalCenter: parent.verticalCenter
                    }
                    Loader{
                        anchors.verticalCenter: parent.verticalCenter
                        anchors.right: parent.right
                        anchors.rightMargin: 10
                        sourceComponent: root.runSaveTriggerIcon
                    }
                    MouseArea{
                        id: saveeSelectArea
                        anchors.fill: parent
                        hoverEnabled: true
                        onClicked: {
                            triggerTypeList.close()
                            root.triggerSelected(Document.RunOnSave)
                        }
                    }
                }

                Rectangle{
                    width: parent.width
                    height: 30
                    color: manualSelectArea.containsMouse ? root.popupMenuStyle.highlightBackgroundColor : 'transparent'

                    Input.Label{
                        text: qsTr("Manual")
                        textStyle: manualSelectArea.containsMouse ? root.popupMenuStyle.highlightTextStyle : root.popupMenuStyle.textStyle
                        anchors.left: parent.left
                        anchors.leftMargin: 10
                        anchors.verticalCenter: parent.verticalCenter
                    }
                    Loader{
                        anchors.verticalCenter: parent.verticalCenter
                        anchors.right: parent.right
                        anchors.rightMargin: 10
                        sourceComponent: root.runDisabledTriggerIcon
                    }
                    MouseArea{
                        id: manualSelectArea
                        anchors.fill: parent
                        hoverEnabled: true
                        onClicked: {
                            triggerTypeList.close()
                            root.triggerSelected(Document.RunManual)
                        }
                    }
                }
            }
        }
    }
}
