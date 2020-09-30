import QtQuick 2.0
import QtQuick.Controls 2.10

Menu{
    id: contextMenu
    property Component menuItemFactory: MenuItem{}

    function clearItems(){
        while (count)
            takeItem(0)
    }

    function show(model){
        for ( var i = 0; i < model.length; ++i ){
            var menuItem = menuItemFactory.createObject()
            menuItem.text = model[i].name
            // menuItem.enabled = true
            menuItem.triggered.connect(model[i].action)
            // menuItem.shortcut = model[i].shortcut ? model[i].shortcut : ''
            contextMenu.insertItem(i, menuItem)
        }

        contextMenu.popup()
    }

    onAboutToHide: {
        clearItems()
    }

}
