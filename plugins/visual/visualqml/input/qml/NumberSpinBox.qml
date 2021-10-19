import QtQuick 2.11

Item{
    id: root
    width: 100
    height: 20
    clip: true
    
    property QtObject style: NumberSpinBoxStyle{}

    function increment(){
        var val = parseFloat(numberSpinboxInput.text)
        if ( !decimals )
            val = Math.floor(val)
        if ( val < to ){
            val += root.incrementStep
            numberSpinboxInput.text = val
            value = val
        }
        numberSpinboxInput.text = value
    }

    function decrement(){
        var val = parseFloat(numberSpinboxInput.text)
        if ( !decimals )
            val = Math.floor(val)
        if ( val > from ){
            val -= root.decrementStep
            numberSpinboxInput.text = val
            value = val
        }
        numberSpinboxInput.text = value
    }

    function commitValue(){
        var val = parseFloat(numberSpinboxInput.text)
        if ( !decimals )
            val = Math.floor(val)
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
    property bool decimals : true
    
    property double incrementStep: 1
    property double decrementStep: 1
    
    property double buttonWidth: 20

    function initializeValue(val){
        root.value = val
        numberSpinboxInput.text = val
    }

    property Item __incrementButton: null
    property Item __decrementButton : null

    InputBox{
        id: numberSpinboxInput

        width: parent.width - parent.buttonWidth
        anchors.verticalCenter: parent.verticalCenter

        height: parent.height
        margins: 3
        style: root.style.inputBox
        text: parent.value

        Keys.onReturnPressed: { root.commitValue() }
    }
    
    function __initialize(){
        if ( __incrementButton ){
            __incrementButton.destroy()
            __incrementButton = null
        }
        __incrementButton = style.incrementButton.createObject(root)
        __incrementButton.parent = root
        __incrementButton.anchors.right = root.right
        __incrementButton.height = Qt.binding(function(){ return root.height / 2 })
        __incrementButton.width = root.buttonWidth
        __incrementButton.clicked.connect(function(){
            root.increment()
        })

        if ( __decrementButton ){
            __decrementButton.destroy()
            __decrementButton = null
        }

        __decrementButton = style.decrementButton.createObject(root)
        __decrementButton.parent = root
        __decrementButton.anchors.right = root.right
        __decrementButton.y = Qt.binding(function(){ return root.height / 2 })
        __decrementButton.height = Qt.binding(function(){ return root.height / 2 })
        __decrementButton.width = root.buttonWidth
        __decrementButton.clicked.connect(function(){
            root.decrement()
        })
    }
    
    Component.onCompleted : {
        __initialize()
    }

    Rectangle{
    }
}
