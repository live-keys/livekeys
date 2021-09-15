import QtQuick 2.3
import base 1.0

Act{
    property string text: ''
    property string value: ''
    property string newValue: ''

    run: function(text, value, newValue){
        return text.replace(new RegExp(value, 'g'), newValue)
    }
    args: ["$text", "$value", "$newValue"]
}
