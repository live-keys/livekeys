import QtQuick 2.3
import base 1.0

Act{
    property string text: ''
    property string value: ''
    property string newValue: ''

    run: function(text, value, newValue){
        return text.replaceAll(value, newValue)
    }
    args: ["$text", "$value", "$newValue"]
}
