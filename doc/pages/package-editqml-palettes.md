# Palettes in editqml


To capture the palette functions object, we do:

```js
var paletteFunctions = lk.layers.workspace.extensions.editqml.paletteFunctions
```


#### Querying the user for a palette to open at a certain position

```js
paletteFunctions.userOpenPaletteAtPosition(editor, position, function(palette){
    if ( palette )
        console.log("PALETTE OPENED:" + palette.name)
    var paletteBox = palette.editFragment.visualParent // Access to the paletteBox
})
```

#### Opening a specific palette at a position

```js
var palette = paletteFunctions.userOpenPaletteAtPosition(editor, "PALETTE_NAME", position)
```

#### Querying the user to shape a certain position into a palette

```js
paletteFunctions.userShapePaletteAtPosition(editor, position, function(palette){
    if ( palette )
        console.log("PALETTE OPENED:" + palette.name)
    var paletteBox = palette.editFragment.visualParent // Access to the paletteBox
})
```


#### Shaping at a certain position

```js
var result = paletteFunctions.shapePaletteAtPosition(editor, position)
console.log(result.objectContainer) // if shaping was done on an object
console.log(result.palette) // the palette that was shaped
```
