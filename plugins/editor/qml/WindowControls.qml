import QtQuick 2.5
import QtQuick.Dialogs 1.2
import QtQuick.Controls 1.2

QtObject{
    property FileDialog saveFileDialog: null
    property FileDialog openFileDialog: null
    property FileDialog openDirDialog: null
    property var messageDialog: null
    property QtObject runSpace: null
    property Item editSpace: null
    property Timer createTimer: null
    property Component paletteBoxFactory: null
    property Item activePane : null
    property Item activeItem : null
    property Item navEditor: null
    property int codingMode: null
    property int prevWindowState: null

    function setActiveItem(item, pane){
        activeItem = item
        var p = pane ? pane : item
        while ( p !== null ){
            if ( p.objectName === 'editor' || p.objectName === 'project' || p.objectName === 'viewer' ){
                activePane = p
                return
            }
            p = p.parent
        }
    }

    function activateItem(item, pane){
        if ( activeItem && activeItem !== item ){
            activeItem.focus = false
        }

        activeItem = item
        activeItem.forceActiveFocus()
        var p = pane ? pane : item
        while ( p !== null ){
            if ( p.objectName === 'editor' || p.objectName === 'project' || p.objectName === 'viewer' ){
                activePane = p
                return
            }
            p = p.parent
        }
    }
}
