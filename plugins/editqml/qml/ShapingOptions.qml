import QtQuick 2.3
import editor 1.0

QtObject{
    property Component addBoxFactory: Component{ AddQmlBox{} }
    property Component propertyContainerFactory: Component{ PropertyContainer{} }
    property Component paletteContainerFactory: Component{ PaletteContainer{} }
    property Component paletteGroupFactory : Component{ PaletteGroup{} }

    function createAddBox(parent){
        return addBoxFactory.createObject(parent)
    }

    function createPropertyContainer(parent){
        return propertyContainerFactory.createObject(parent)
    }

    function createPaletteGroup(parent){
        return paletteGroupFactory.createObject(parent)
    }

    function createPaletteContainer(parent){
        return paletteContainerFactory.createObject(parent)
    }

}
