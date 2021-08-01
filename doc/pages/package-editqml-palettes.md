# Palettes in editqml


To capture the palette functions object, we do:

```js
var paletteControls = lk.layers.workspace.extensions.editqml.paletteControls
```


#### Querying the user for a palette to open at a certain position

```js
paletteControls.userOpenPaletteAtPosition(editor, position, function(palette){
    if ( palette )
        console.log("PALETTE OPENED:" + palette.name)
    var paletteBox = palette.editFragment.visualParent // Access to the paletteBox
})
```

#### Opening a specific palette at a position

```js
var palette = paletteControls.userOpenPaletteAtPosition(editor, "PALETTE_NAME", position)
```

#### Querying the user to shape a certain position into a palette

```js
paletteControls.userShapePaletteAtPosition(editor, position, function(palette){
    if ( palette )
        console.log("PALETTE OPENED:" + palette.name)
    var paletteBox = palette.editFragment.visualParent // Access to the paletteBox
})
```


#### Shaping at a certain position

```js
var result = paletteControls.shapePaletteAtPosition(editor, position)
console.log(result.objectContainer) // if shaping was done on an object
console.log(result.palette) // the palette that was shaped
```
