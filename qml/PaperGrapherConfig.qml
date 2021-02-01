import QtQuick 2.3
import QtQuick.Controls 2.4
import QtQuick.Dialogs 1.2

QtObject{


    property QtObject box: QtObject{
        property color background: "#333"
        property color borderColor: "#555"
        property double borderWidth: 1
    }

    property Component label: Text{
        font.family: "Arial, Helvetica, sans-serif"
        font.weight: Font.Normal
        font.pixelSize: 12
        color: 'white'
        text: "Sample"
    }

    property Component boolInput: Item{
        width: boolCheckBox.width
        height: boolCheckBox.height
        x: 5

        CheckBox {
            id: boolCheckBox
            checked: false
            width: 20
            height: 20
            padding: 0

            indicator: Rectangle {
                implicitWidth: 16
                implicitHeight: 16
                x: boolCheckBox.leftPadding
                y: parent.height / 2 - height / 2
                radius: 3
                color: "#444"
                border.color: boolCheckBox.down ? "#777" : "#777"

                Rectangle {
                    width: 10
                    height: 10
                    x: 3
                    y: 3
                    radius: 2
                    color: boolCheckBox.down ? "#777" : "#777"
                    visible: boolCheckBox.checked
                }
            }

            onCheckedChanged: parent.valueChanged(checked)
        }
        signal valueChanged(bool value)

        function setup(options){
            if ( options.hasOwnProperty("value") )
                boolCheckBox.checked = options.value
        }
    }

    property Component intInput : Item{

        Rectangle{
            id: spinbox
            anchors.fill: parent
            border.color: '#777'
            border.width: 1
            color: "#555"
            clip: true

            function increment(){
                var val = parseInt(spinboxInput.text)
                if ( val < to ){
                    val++
                    spinboxInput.text = val
                    value = val
                }
                spinboxInput.text = value
            }

            function decrement(){
                var val = parseInt(spinboxInput.text)
                if ( val > from ){
                    val--
                    spinboxInput.text = val
                    value = val
                }
                spinboxInput.text = value
            }

            function commitValue(){
                var val = parseInt(spinboxInput.text)
                if ( !val ){
                    spinboxInput.text = value
                } else if ( val > to ){
                    value = to
                } else if ( val < from ){
                    value = from
                } else {
                    value = val
                }
            }

            property int from: 0
            property int to: 100
            property int value: 0
            onValueChanged: parent.valueChanged(value)

            TextInput{
                id: spinboxInput

                anchors.left: parent.left
                anchors.leftMargin: 2
                width: parent.width - 22
                anchors.verticalCenter: parent.verticalCenter

                height: parent.height - 5
                color: 'white'
                selectByMouse: true
                text: parent.value

                Keys.onReturnPressed: { spinbox.commitValue() }
            }

            Rectangle{
                anchors.top: parent.top
                anchors.right: parent.right
                width: 20
                height: parent.height / 2
                color: "#444"
                Text{
                    anchors.centerIn: parent
                    color: 'white'
                    text: '+'
                }
                MouseArea{
                    anchors.fill: parent
                    onClicked: spinbox.increment()
                }
            }
            Rectangle{
                anchors.right: parent.right
                anchors.bottom: parent.bottom
                width: 20
                height: parent.height / 2
                color: "#444"
                Text{
                    anchors.centerIn: parent
                    color: 'white'
                    text: '-'
                }

                MouseArea{
                    anchors.fill: parent
                    onClicked: spinbox.decrement()
                }
            }
        }


        signal valueChanged(int value)

        function setup(options){
            if ( options.hasOwnProperty('value') && options.value !== undefined ){
                spinbox.value = options.value
                spinboxInput.text = options.value
            }
            if ( options.hasOwnProperty('min') )
                spinbox.from = options.min
            if ( options.hasOwnProperty('max') )
                spinbox.to = options.max
        }
    }

    property Component numberInput: Item{


        Rectangle{
            id: numberSpinbox
            anchors.fill: parent
            border.color: '#777'
            border.width: 1
            color: "#555"
            clip: true

            function increment(){
                var val = parseFloat(numberSpinboxInput.text)
                if ( val < to ){
                    val += 1
                    numberSpinboxInput.text = val
                    value = val
                }
                numberSpinboxInput.text = value
            }

            function decrement(){
                var val = parseFloat(numberSpinboxInput.text)
                if ( val > from ){
                    val -= 1
                    numberSpinboxInput.text = val
                    value = val
                }
                numberSpinboxInput.text = value
            }

            function commitValue(){
                var val = parseFloat(numberSpinboxInput.text)
                if ( !val ){
                    numberSpinboxInput.text = value
                } else if ( val > to ){
                    value = to
                } else if ( val < from ){
                    value = from
                } else {
                    value = val
                }
            }

            property int from: 0
            property int to: 100
            property int value: 0
            onValueChanged: parent.valueChanged(value)

            TextInput{
                id: numberSpinboxInput

                anchors.left: parent.left
                anchors.leftMargin: 2
                width: parent.width - 22
                anchors.verticalCenter: parent.verticalCenter

                height: parent.height - 5
                color: 'white'
                selectByMouse: true
                text: parent.value

                Keys.onReturnPressed: { numberSpinbox.commitValue() }
            }

            Rectangle{
                anchors.top: parent.top
                anchors.right: parent.right
                width: 20
                height: parent.height / 2
                color: "#444"
                Text{
                    anchors.centerIn: parent
                    color: 'white'
                    text: '+'
                }
                MouseArea{
                    anchors.fill: parent
                    onClicked: numberSpinbox.increment()
                }
            }
            Rectangle{
                anchors.right: parent.right
                anchors.bottom: parent.bottom
                width: 20
                height: parent.height / 2
                color: "#444"
                Text{
                    anchors.centerIn: parent
                    color: 'white'
                    text: '-'
                }

                MouseArea{
                    anchors.fill: parent
                    onClicked: numberSpinbox.decrement()
                }
            }
        }

        signal valueChanged(real value)

        function setup(options){
        }
    }

    property Component intDropDown : Item{

        ComboBox {
            id: control
            editable: true
            height: 20
            width: 40
            font.pixelSize: 10
            validator: IntValidator{ bottom: 0; top: 100 }
            currentIndex: 10
            leftPadding: 2
            model: ['0%', '10%', '20%', '30%', '40%', '50%', '60%', '70%', '80%', '90%', '100%']

            indicator: Canvas {
                id: canvas
                x: control.width - width - 2
                y: control.topPadding + (control.availableHeight - height) / 2
                width: 12
                height: 8
                contextType: "2d"

                Connections {
                    target: control
                    function onPressedChanged() { canvas.requestPaint(); }
                }

                onPaint: {
                    context.reset();
                    context.moveTo(0, 0);
                    context.lineTo(width, 0);
                    context.lineTo(width / 2, height);
                    context.closePath();
                    context.fillStyle = control.pressed ? "white" : "white";
                    context.fill();
                }
            }

            contentItem: TextInput {
                selectByMouse: true
                text: control.displayText
                font: control.font
                color: control.pressed ? "white" : "white"
                verticalAlignment: Text.AlignVCenter
                validator: control.validator
                Keys.onReturnPressed: {
                    var val = parseInt(text)
                    if ( val > 100 )
                        val = 100
                    if ( val < 0 )
                        val = 0

                    control.displayText = val + '%'
                    event.accepted = true
                    control.accepted()
                }
            }

            delegate: ItemDelegate {
                id: itemDelegate
                width: control.width
                height: 20
                padding: 0
                contentItem: Rectangle{
                    color: itemDelegate.hovered ? '#666' : '#555'
                    Text {
                        text: modelData
                        color: 'white'
                        font: control.font
                        elide: Text.ElideRight
                    }
                }
                highlighted: control.highlightedIndex === index
            }

            background: Rectangle {
                implicitWidth: 120
                implicitHeight: 40
                border.color: control.pressed ? "black" : "black"
                border.width: control.visualFocus ? 2 : 1
                radius: 2
                color: "#555"
            }

            onAccepted: {
                valueChanged(displayText.replace('%', ''))
            }
            onCurrentIndexChanged: {
                var val = model[currentIndex]
                valueChanged(val.replace('%', ''))
            }

            signal valueChanged(var value)
            onValueChanged: {
                if ( value !== '' ){
                    control.parent.valueChanged(value)
                }
            }
        }

        signal valueChanged(var value)

    }

    property Component textInput: Item{

        TextField {
            id: textField
            anchors.fill: parent

            height: 25
            width: 150
            font.pixelSize: 12
            padding: 0
            color: "#fff"
            background: Rectangle {
                implicitWidth: 200
                implicitHeight: 40
                color: "#555"
                border.color: "#777"
            }

            placeholderText: qsTr("Enter text")
            onTextChanged: parent.valueChanged(text)
        }

        signal valueChanged(var value)

        property string currentValue: textField.text

        function setup(options){
            if ( options.hasOwnProperty('value') )
                textField.text = options.value
        }
    }

    property Component dropDownInput: Item{

        ComboBox{
            id: dropDownBox
            anchors.fill: parent

            height: 25
            width: 70
            font.pixelSize: 9

            contentItem: Item{
                Text{
                    anchors.fill: parent
                    anchors.margins: 1
                    anchors.leftMargin: 3
                    text: dropDownBox.displayText
                    font: dropDownBox.font
                    color: dropDownBox.pressed ? "white" : "white"
                    verticalAlignment: Text.AlignVCenter
                }
            }

            delegate: ItemDelegate {
                id: dropDownBoxDelegate
                width: dropDownBox.width
                height: 20
                padding: 0
                contentItem: Rectangle{
                    width: dropDownBox.width + 20
                    color: dropDownBoxDelegate.hovered ? '#666' : '#333'
                    Text {
                        text: modelData
                        color: 'white'
                        font: dropDownBox.font
                        elide: Text.ElideRight
                        anchors.verticalCenter: parent.verticalCenter
                        anchors.left: parent.left
                        anchors.leftMargin: 2
                    }
                }
                highlighted: dropDownBox.highlightedIndex === index
            }

            background: Rectangle {
                implicitWidth: 120
                implicitHeight: 40
                border.color: dropDownBox.pressed ? "#777" : "#777"
                border.width: dropDownBox.visualFocus ? 2 : 1
                radius: 2
                color: "#555"
            }

            indicator: Canvas {
                id: dropDownBoxCanvas
                x: dropDownBox.width - width - 7
                y: dropDownBox.topPadding + (dropDownBox.availableHeight - height) / 2
                width: 8
                height: 6
                contextType: "2d"

                Connections {
                    target: dropDownBox
                    function onPressedChanged() { dropDownBoxCanvas.requestPaint(); }
                }

                onPaint: {
                    if ( !context )
                        return
                    context.reset();
                    context.moveTo(0, 0);
                    context.lineTo(width, 0);
                    context.lineTo(width / 2, height);
                    context.closePath();
                    context.fillStyle = dropDownBox.pressed ? "white" : "white";
                    context.fill();
                }
            }

            model: []

            onActivated: {
                parent.selectedValue = dropDownBox.model[index]
                parent.valueChanged(index, dropDownBox.model[index])
            }
        }

        property var selectedValue: ''

        signal valueChanged(int index, var value)

        function setup(options){
            if ( options.model )
                dropDownBox.model = options.model
            if ( options.hasOwnProperty('index') ){
                selectedValue = dropDownBox.model[options.index]
                dropDownBox.currentIndex = options.index
            }
        }
    }

    property Component colorInput: Item{
        id: colorInput

        ColorDialog {
            id: colorDialog
            title: "Select a color"
            visible: false
            showAlphaChannel: true

            onAccepted: { colorInput.valueChanged(colorDialog.color) }
            Component.onCompleted: visible = false
        }

        function setup(opt){
            colorDialog.visible = true
            if ( opt.color )
                colorDialog.color = opt.color
        }

        signal valueChanged(var value)

    }

}
