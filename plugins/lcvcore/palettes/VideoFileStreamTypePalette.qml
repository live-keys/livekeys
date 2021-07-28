import QtQuick 2.3
import QtQuick.Controls 1.2
import QtQuick.Controls.Styles 1.4
import base 1.0
import editor 1.0
import live 1.0
import lcvcore 1.0
import visual.input 1.0 as Input
import workspace.icons 1.0 as Icons
import workspace 1.0

CodePalette{
    id: palette

    type : "qml/lcvcore#VideoFile.streamType"

    property QtObject theme: lk.layers.workspace.themes.current

    item: Item{
        height: 30 + (wrapSelection.visible ? wrapSelection.height : 0)
        width: 200

        Input.DropDown{
            id: streamType
            anchors.left: parent.left
            anchors.leftMargin: 3
            width: parent.width
            style: palette.theme.dropDownStyle

            model: ['Direct', 'Wrap']

            onActivated: {
                if ( index === 0 ){
                    palette.assignValue('Direct', [])
                } else {
                    palette.assignValue('Wrap', wrapSelection.model)
                }
            }
        }

        DropDownList{
            id: wrapSelection
            anchors.top: parent.top
            anchors.topMargin: 25
            selectionModel: ['image', 'frameNumber']
            visible: streamType.currentIndex === 1

            onReady: {
                palette.assignValue('Wrap', wrapSelection.model)
            }

            whenEntrySelected: function(index, value){
                var selectionModel = wrapSelection.selectionModel
                selectionModel.splice(index,1)
                wrapSelection.selectionModel = selectionModel

                var model = wrapSelection.model
                model.push(value)
                wrapSelection.model = model
            }
            whenEntryDeleted: function(index){
                var model = wrapSelection.model

                var selectionModel = wrapSelection.selectionModel
                selectionModel.push(model[index])
                wrapSelection.selectionModel = selectionModel


                model.splice(index, 1)
                wrapSelection.model = model
            }
        }
    }

    function assignValue(streamType, streamValues){
        if ( streamType === 'Direct' ){
            palette.value = 'qml/lcvcore#Mat'
        } else {
            palette.value = streamValues
        }
        palette.editFragment.write(palette.value)
    }

    function initFromValue(value){
        if (typeof value === 'string' || value instanceof String){
            streamType.currentIndex = 0
        } else {
            streamType.currentIndex = 1

            var model = wrapSelection.model
            var selectionModel = wrapSelection.selectionModel

            for ( var i = 0; i < value.length; ++i ){

                var streamValue = value[i]
                model.push(streamValue)

                for ( var j = 0; j < selectionModel.length; ++j ){
                    if ( selectionModel[j] === streamValue ){
                        selectionModel.splice(j, 1)
                        break
                    }
                }
            }

            wrapSelection.model = model
            wrapSelection.selectionModel = selectionModel
        }
    }

    onInit: {
        palette.initFromValue(value)
        palette.editFragment.whenBinding = function(){
            editFragment.write(palette.value)
        }
    }
    onValueFromBindingChanged: {
        palette.initFromValue(value)
    }
}
