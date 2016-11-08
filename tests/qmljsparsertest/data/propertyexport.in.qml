import QtQuick 2.3

Item {
    id: root
    property int intProperty
    property string stringProperty
    property Rectangle rectangleProperty

    signal exportSignal(string stringArgument, int intArgument)

    Rectangle{
        property int parentIntProperty

        Item{
            id: scope

            property var scopeVarProperty

            width : parent.width
            onWidthChanged : {
                console.log("width changed")
            }
        }
    }
}
