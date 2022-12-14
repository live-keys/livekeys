import QtQuick 2.3
import workspace.icons 1.0 as Icons
import visual.shapes 1.0
import visual.input 1.0 as Input

Row{
    id: modelSplitterRow
    spacing: 5

    property alias model: modelSplitter.model
    property color iconColor: 'white'
    property QtObject textStyle: Input.TextStyle{}
    property QtObject fileIcon: null
    property QtObject componentIcon: null
    property QtObject watcherIcon: null
    property QtObject stackIcon: null

    Repeater{
        id: modelSplitter

        Item{
            anchors.top: parent.top
            height: 25
            width: pathIconLoader.width + pathText.width + pathSplitter.width + (pathIconLoader.visible ? 18 : 8 )

            property int type: 0

            Loader{
                id: pathIconLoader
                anchors.verticalCenter: parent.verticalCenter

                sourceComponent: {
                    if ( modelData[0] === 'F' )
                        return modelSplitterRow.fileIcon
                    else if ( modelData[0] === 'C' )
                        return modelSplitterRow.componentIcon
                    else if ( modelData[0] === 'W' ){
                        return modelSplitterRow.watcherIcon
                    } else if ( modelData[0] === 'I' ){
                        return modelSplitterRow.stackIcon
                    }

                    return ""
                }
                onItemChanged: {
                    if ( item.width > 12 )
                        item.width = 12
                    if ( item.height > 12 )
                        item.height = 12
                    if ( item.color )
                        item.color = modelSplitterRow.iconColor
                }
            }

            Input.Label{
                id: pathText
                anchors.left: pathIconLoader.right
                anchors.leftMargin: pathIconLoader.visible ? 8 : 0
                anchors.verticalCenter: parent.verticalCenter
                textStyle: modelSplitterRow.textStyle
                text: modelData.substr(2)
            }

            Text{
                id: pathSplitter
                anchors.verticalCenter: parent.verticalCenter
                font.family: 'Open Sans'
                font.pixelSize: modelSplitterRow.textStyle.fontSize + 2
                font.weight: Font.Bold
                color: modelSplitterRow.textStyle.color
                anchors.left: pathText.right
                anchors.leftMargin: 8
                text: '>'
                visible: modelSplitter.model.length !== index + 1
            }
        }
    }
}

