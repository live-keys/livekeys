import QtQuick 2.3
import workspace 1.0
import visual.input 1.0 as Input

Tool{
    id: root
    toolLabel: 'Resize'
    property QtObject theme: lk.layers.workspace.themes.current

    property QtObject labelInfoStyle: Input.TextStyle{}
    property Component applyButton : null
    property Component cancelButton : null
    property Component preserveAspectButton: null

    signal apply(double width, double height)
    signal cancel()

    property bool preserveAspect: true

    infoBarContent: Item{
        anchors.fill: parent

        Input.Label{
            id: selectionInfo
            anchors.verticalCenter: parent.verticalCenter
            anchors.left: parent.left
            anchors.leftMargin: 0
            textStyle: root.labelInfoStyle
            text: 'Resize'
        }

        Input.InputBox{
            id: widthInput
            anchors.left: parent.left
            anchors.leftMargin: 40
            anchors.top: parent.top
            anchors.topMargin: 2
            margins: 2

            width: 40
            height: 20
            text: ''

            style: theme.inputStyle

            onTextChanged: {
                if ( root.preserveAspect && widthInput.inputActiveFocus ){
                    var dims  = root.canvas.imageView.image.dimensions()
                    var aspect = dims.width / dims.height
                    var newWidth = parseInt(widthInput.text)
                    var height = newWidth / aspect
                    heightInput.text = Math.round(height)
                }
            }
        }

        Loader{
            anchors.top: parent.top
            anchors.topMargin: 2
            anchors.left: parent.left
            anchors.leftMargin: 85
            width: 20
            height: 20

            property bool containsMouse: preserveAspectMouseArea.containsMouse

            sourceComponent: root.preserveAspectButton
        }

        MouseArea{
            id: preserveAspectMouseArea
            anchors.top: parent.top
            anchors.topMargin: 2
            anchors.left: parent.left
            anchors.leftMargin: 85
            width: 30
            height: 20
            hoverEnabled: true
            onClicked: root.preserveAspect = !root.preserveAspect
        }


        Input.InputBox{
            id: heightInput
            anchors.left: parent.left
            anchors.leftMargin: 110
            anchors.top: parent.top
            anchors.topMargin: 2
            margins: 2

            width: 40
            height: 20
            text: ''

            style: theme.inputStyle

            onTextChanged: {
                if ( root.preserveAspect && heightInput.inputActiveFocus ){
                    var dims  = root.canvas.imageView.image.dimensions()
                    var aspect = dims.width / dims.height
                    var newHeight = parseInt(heightInput.text)
                    var width = newHeight * aspect
                    widthInput.text = Math.round(width)
                }
            }
        }


        Input.Button{
            anchors.left: parent.left
            anchors.leftMargin: 170
            width: 25
            height: 25
            content: root.applyButton
            onClicked: root.apply(parseInt(widthInput.text), parseInt(heightInput.text))
        }
        Input.Button{
            anchors.left: parent.left
            anchors.leftMargin: 198
            width: 25
            height: 25
            content: root.cancelButton
            onClicked: root.cancel()
        }
    }

    activate: function(){
        var dims  = root.canvas.imageView.image.dimensions()
        widthInput.text = dims.width
        heightInput.text = dims.height
    }

    deactivate: function(){
    }

}
