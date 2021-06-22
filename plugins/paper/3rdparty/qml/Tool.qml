import QtQuick 2.0

QtObject{

    property QtObject paperCanvas: null
    property QtObject paperGrapherLoader: null
    property string toolLabel: ''
    property Item infoBarContent: null
    property Item optionsPanelContent: null

    property var activate: function(){}
    property var deactivate: function(){}
    property var mouseDown: function(event){}
    property var mouseUp: function(event){}
    property var mouseMove: function(event){}
    property var mouseDrag: function(event){}
    property var mouseDoubleClicked: function(event){}

}
