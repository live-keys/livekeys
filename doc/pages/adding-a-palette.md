# Adding a palette

To add a palette, we have to make sure that LiveKeys is made aware that the palette exists. That's done through the appropriate `live.plugin.json` file for each plugin, under the `palettes` key. An example is given below.

```json
{
    "name": "editor",
    "package" : ".",
    "palettes" : {
        "palettes/EditPalette.qml" : "edit/qml",
        "palettes/IntPalette.qml": "qml/int",
        "palettes/IntHistoryPlotPalette.qml": "qml/int",
        "palettes/DoublePalette.qml": "qml/double",
        "palettes/DoubleHistoryPlotPalette.qml": "qml/double",
        "palettes/ColorPalette.qml": "qml/color"
    }
}
```

We give the path to the palette file relative to the `live.plugin.json` as the key, and the type that the palette supports as the value. For example, both the `DoublePalette` and `DoubleHistoryPlotPalette` can be used to display a variable of the `double` type.

After registering the palette in such a manner, we create the appropriate Qml file. The palette itself has to be of CodePalette type.

```qml
// imports etc.
CodePalette {
	...
	type: "qml/int"
	item: Rectangle { ... }
	onInit: ...
}
```

The code palette has the `type` property, which should match the one given inside the `live.plugin.json` file.  
The `item` property is the actual item that will be displayed when we open up a palette.   
The palette can be initialized in the `onInit` handler e.g. that's how we would set it up to display the correct value.  
Palettes get an extension called QmlCodeConverter which represents the Qml part for the palette.  
Palettes are used as a visual tool to control and change the value of a property, through sliders, scrolls and other types of controls. Another possible uses for palettes is non-visual, when they are used simply to bind the related value to some code. QmlCodeConverter comes into play in both cases, since we use to to provide the binding and write the value that the palette receives into the code.


