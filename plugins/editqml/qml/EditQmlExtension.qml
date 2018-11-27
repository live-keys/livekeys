import QtQuick 2.3
import editor 1.0
import editqml 1.0

LiveExtension{
    id: root

    globals : ProjectQmlExtension{}
    interceptLanguage : function(document, handler, ext){
        var extLower = ext.toLowerCase()
        if ( extLower === 'js' || extLower === 'qml' || document.file.path === '' ){
            return globals.createHandler(document, handler)
        }
        return null;
    }

    objectName : "editqml"

    property Component addBoxFactory: Component{ AddQmlBox{} }

    commands : {
        "adjust" : function(){
            var activePane = livecv.windowControls().activePane
            if ( activePane.objectName === 'editor' &&
                 activePane.document &&
                 ( activePane.document.file.path.endsWith('.qml') || activePane.document.file.path === '' ) )
            {
                activePane.adjust()
            }
        },
        "add" : function(){
            var activePane = livecv.windowControls().activePane
            if ( activePane.objectName === 'editor' &&
                 activePane.document &&
                 ( activePane.document.file.path.endsWith('.qml') || activePane.document.file.path === '' ) )
            {
                var addContainer = activePane.documentHandler.codeHandler.getAddOptions(activePane.textEdit.cursorPosition)
                if ( !addContainer )
                    return

                var rect = activePane.getCursorRectangle()
                var cursorCoords = activePane.cursorWindowCoords()
                var addBoxItem = addBoxFactory.createObject()
                addBoxItem.addContainer = addContainer

                var addBox = livecv.windowControls().editSpace.createEditorBox(
                    addBoxItem, rect, cursorCoords, livecv.windowControls().editSpace.placement.bottom
                )
                addBox.color = 'transparent'
                addBoxItem.cancel = function(){
                    addBox.destroy()
                }
                addBoxItem.accept = function(data){
                    if ( addBoxItem.activeIndex === 0 ){
                        activePane.documentHandler.codeHandler.addProperty(addContainer.propertyModel.addPosition, data)
                    } else {
                        activePane.documentHandler.codeHandler.addItem(addContainer.itemModel.addPosition, data)
                    }
                    addBox.destroy()
                }

                addBoxItem.assignFocus()
                livecv.windowControls().setActiveItem(addBox, activePane)

            }
        },
        'shape' : function(){

        }
    }

    keyBindings : {
        "alt+a" : "editqml.adjust",
        "alt+z" : "editqml.add"
    }

    interceptMenu : function(item){
        if ( item.objectName === 'editor' && item.document ){
            if ( item.document.file.path.endsWith('.qml') || item.document.file.path === '' ){
                var cursorInfo = item.documentHandler.cursorInfo(
                    item.textEdit.selectionStart, item.textEdit.selectionEnd - item.textEdit.selectionStart
                );

                return [
                    {
                        name : "Edit",
                        action : function(){
                            item.documentHandler.edit(item.textEdit.cursorPosition, item.windowControls.runSpace.item)
                        },
                        enabled : cursorInfo.canEdit
                    }, {
                        name : "Adjust",
                        action : function(){
                            item.adjust()
                        },
                        enabled : cursorInfo.canAdjust
                    }, {
                        name : "Bind",
                        action : function(){
                            item.documentHandler.bind(
                                item.textEdit.selectionStart,
                                item.textEdit.selectionEnd - item.textEdit.selectionStart,
                                item.windowControls.runSpace.item
                            )
                        },
                        enabled : cursorInfo.canBind
                    }, {
                        name : "Unbind",
                        action : function(){
                            item.documentHandler.unbind(
                                item.textEdit.selectionStart,
                                item.textEdit.selectionEnd - item.textEdit.selectionStart
                            )
                        },
                        enabled : cursorInfo.canUnbind
                    }, {
                        name : "Add",
                        action : function(){
                            root.commands['add']()
                        },
                        enabled : true
                    }
                ]
            }
        }
        return null
    }

}
