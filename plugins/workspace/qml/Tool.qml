import QtQuick 2.0

QtObject{

    property QtObject canvas: null
    property string toolLabel: ''
    property Item infoBarContent: null
    property Item optionsPanelContent: null

    property var activate: function(){}
    property var deactivate: function(){}
    property var mouseDown: function(event){}
    property var mouseUp: function(event){}
    property var mouseDrag: function(event){}

}
