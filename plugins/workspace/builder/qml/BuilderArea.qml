import QtQuick 2.3

Item{
    id: root
    
    property var items: []
    property var selectedItems: []
    
    property Component highlight: BuilderHighlight{}
    property Item highlightItem: null
    
    signal itemsLayoutChanged(var changes)
    signal rightClicked(var mouse, var context)
    
    function itemAtPosition(x, y){
        var foundItem = null
        var foundItemRect = null
        var foundItemData  = null
        for ( var i = 0; i < root.items.length; ++i ){
            var item = root.items[i].item
            var itemRect = item.mapToItem(root, 0, 0, item.width, item.height)
            if ( x >= itemRect.x && x < itemRect.x + itemRect.width
               && y >= itemRect.y && y < itemRect.y + itemRect.height )
            {
                foundItem = item
                foundItemRect = itemRect
                foundItemData = root.items[i].hasOwnProperty('data') ? root.items[i].data : null
            }
        }
        return foundItem ? { item: foundItem, rect: foundItemRect, data: foundItemData } : null
    }
    
    MouseArea{
        id: control
        anchors.fill: parent
        hoverEnabled: true
        acceptedButtons: Qt.LeftButton | Qt.RightButton
        
        onPositionChanged: {
            var result = root.itemAtPosition(mouse.x, mouse.y)
            var resultValid = result && result.item
            if ( resultValid && root.selectedItems.length === 1 ){
                if ( root.selectedItems[0].item === result.item )
                    resultValid = false
            }
            
            if ( resultValid ){
                if ( !root.highlightItem ){
                    root.highlightItem = root.highlight.createObject(root)
                }
                root.highlightItem.x = result.rect.x
                root.highlightItem.y = result.rect.y
                root.highlightItem.width = result.rect.width
                root.highlightItem.height = result.rect.height
            } else {
                if ( root.highlightItem )
                    root.highlightItem.destroy()
            }
        }
        
        onPressed: {
            var result = root.itemAtPosition(mouse.x, mouse.y)
            
            if ( result && result.item ){
                if ( root.selectedItems.length === 1 && root.selectedItems[0].item === result.item )
                    return
                guides.x = result.rect.x
                guides.y = result.rect.y
                guides.width = result.rect.width
                guides.height = result.rect.height
                root.selectedItems = [result]
            } else {
                guides.width = 0
                guides.height = 0
                root.selectedItems = []
            }
        }
        onReleased: {
            if (mouse.button == Qt.RightButton){
                root.rightClicked(mouse, root.selectedItems)
            } else {
                
            }
        }
    }
    
    BuilderGuides{
        id: guides
        onRegionModified: {
            var changes = {}
            if ( root.selectedItems.length === 1 ){
                var selected = root.selectedItems[0]
                if ( guides.x !== selected.rect.x ){
                    var deltaX = selected.rect.x - guides.x
                    selected.rect.x = guides.x
                    selected.item.x -= (deltaX)// * (1.0 / selected.item.scale))
                    changes['x'] = selected.rect.x
                    changes['deltaX'] = deltaX
                }
                if ( guides.y !== selected.rect.y ){
                    var deltaY = selected.rect.y - guides.y
                    selected.rect.y = guides.y
                    selected.item.y -= deltaY
                    changes['y'] = selected.rect.y
                    changes['deltaY'] = deltaY
                }
                if ( guides.width !== selected.rect.width ){
                    var deltaW = selected.rect.width - guides.width
                    selected.rect.width = guides.width
                    selected.item.width -= (deltaW)// * (1.0 / selected.item.scale))
                    changes['width'] = selected.rect.width
                    changes['deltaWidth'] = deltaW
                }
                if ( guides.height !== selected.rect.height ){
                    var deltaH = selected.rect.height - guides.height
                    selected.rect.height = guides.height
                    selected.item.height -= deltaH
                    changes['height'] = selected.rect.height
                    changes['deltaHeight'] = deltaH
                }
                changes['items'] = root.selectedItems
                root.itemsLayoutChanged(changes)
            }
            
            
        }
    }    
}
