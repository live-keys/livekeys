import QtQuick 2.3

QtObject{

    objectName: "objectgraph"

    function removeActiveItem(){
        var activeItem = lk.layers.workspace.panes.activeItem
        if ( activeItem.objectName === 'objectGraph' ){
            if ( activeItem.selectedEdge ){
                activeItem.removeEdge(activeItem.selectedEdge)
            }
        }
    }
}
