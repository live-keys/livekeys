# Pallete Functions in editqml

The `PaletteFunctions` object contains high level functions that work with shaping code in qml documents.
The object is available as part of the editqml extension:

```js
var paletteFunctions = lk.layers.workspace.extensions.editqml.paletteFunctions
```

#### `userOpenPaletteAtPosition(editor, position, callback)`

Finds the declaration at the given position, and finds the available palettes for that declaraiton,
and will open a list and query the user for the available palettes:

```js
paletteFunctions.userOpenPaletteAtPosition(editor, position, function(editFragment, palette){
    if ( palette )
        console.log("PALETTE OPENED:" + palette.name)
    var paletteBox = editFragment.visualParent // Access to the paletteGroup
})
```

The palette will be opened in an external box.

#### `openPaletteAtPosition(editor, paletteName, position, callback)`

Opens a palette by name at the specified `position`.

```js
paletteFunctions.openPaletteAtPosition(editor, "PALETTE_NAME", position, function(editFragment, palette){
    if ( palette )
        console.log("PALETTE OPENED:" + palette.name)
})
```

#### `openEditPaletteAtPosition(editor, position, callback)`

Opens the edit palette at the specified position:

```js
paletteFunctions.openEditPaletteAtPosition(editor, position, function(editFragment, palette){
    if ( palette )
        console.log("PALETTE OPENED:" + palette.name)
})
```

#### `userShapePaletteAtPosition(editor, position, callback)`

The difference between the palette functions above and the shape ones is that shaping will actually transform the section of the code containing the declaration into a section containing the new palette. Shaping works on objects if even if they don't have a palette, since it only transforms them into a UI tree.

```js
paletteFunctions.userShapePaletteAtPosition(editor, position, function(editFragment, palette){
    if ( palette )
        console.log("PALETTE OPENED:" + palette.name)
    var paletteGroup = editFragment.visualParent // Access to the PropertyContainer or to the ObjectContainer, depending on the case
})
```

#### `shapePaletteAtPosition(editor, paletteName, position, callback)`

Shape a specific palette at a specified `position`. The `paletteName` can be an empty string, as then the shaping will open the declaration fragment without a palette. 

```js
paletteFunctions.shapePaletteAtPosition(editor, 'PALETTE_NAME', position, function(editFragment, palette){
    if ( palette )
        console.log("PALETTE OPENED:" + palette.name)
})
```

Some declarations have default palettes configured. They can be opened like this:

```js
paletteFunctions.shapePaletteAtPosition(editor, paletteFunctions.defaultPalette, position, function(editFragment, palette){...})
```

#### `shapeImports(editor, callback)`

Shapes the imports.

```js
paletteFunctions.shapeImports(editor, function(editFragment){
    console.log("Imports shaped")
})
```

#### `shapeRoot(editor, callback)`

Shapes the root of the document.

```js
paletteFunctions.shapeRoot(editor, function(editFragment){
    console.log("Root shaped at:" + editFragment.position())
})
```

#### `openPaletteInPropertyContainer(propertyContainer, paletteName)`

Opens a palette in a given property container. Returns the `PaletteBox` object that wraps the palette.

```js
var paletteBox = paletteFunctions.openPaletteInPropertyContainer(propertyContainer, "PALETTE_NAME")
```

#### `openPaletteInObjectContainer(objectContainer, paletteName)`

Opens a palette in `objectContainer`. Returns the `PaletteBox` object that wraps the palette.

```js
var paletteBox = paletteFunctions.openPaletteInObjectContainer(objectContainer, "PALETTE_NAME")
```

#### `addPropertyToObjectContainer(objectContainer, name, readOnly, position)`

Adds property `name` to `objectContainer` if it doesn't exist. You can force the property to be treated as `readOnly`, otherwise
this property attribute is determined automatically. `position` is optional, in case the user wants to recommend a position the property should be added at.

Returns the `propertyContainer` associated to the added property.

#### `addEventToObjectContainer(objectContainer, name, position)`

Adds event `name` to `objectContainer` if it doesn't exist. `position` is optional, in case the user wants to recommend a position the event should be added at.

Returns the `propertyContainer` associated to the added event.

#### `addObjectToObjectContainer(objectContainer, type, extraProperties, position)`

Creates an object of `type`, and adds it to the object `objectContainer`. The `type` can be specified in multiple ways:

 * As a string, describing the type as it would be declared in the document: `ImageFile`
 * As a string, describing the type as a global reference: `qml/lcvcore#ImageFile`
 * As an object, with `type` and `id` properties, if the user wants to add an id to the type: `{ type: 'qml/lcvcore#ImageFile', id: imageFile }`

`extraProperties` is optional, in case the user wants to add extra properties to the newly created object. The `extraProperties` is an array of objects, where
each object has can have a `name`, `type` and `value`. The `value` is optional, as the value will be assigned with a default value if there's nothing else specified.
`position` is also optional, in case the user wants to recommend a position the event should be added at.

Returns the `objectContainer` associated to the added object.

```js
var childContainer = paletteFunctions.addObjectToObjectContainer(container, 'qml/lcvcore#ImageFile', [{name: 'isOpen', type: 'bool', value: 'false'}])
if ( childContainer ){
    console.log(childContainer)
    console.log(childContainer.editFragment)
}
```

#### `userAddToObjectContainer(objectContainer, handlers)`

Queries the user to select from a set of options (properties, events, objects) to add to the `objectContainer`.
The `handlers` is an object with a set of callbacks defined by the user: `onAccepted` and `onCancelled`

```js
paletteControls.userAddToObjectContainer(container, {
    onCancelled: function(){ console.log('object added') },
    onAccepted: function(){ console.log('operation cancelled') }
})
```

#### `eraseObject(objectContainer)`

Erases the container and the object at `container`.

#### `closeObjectContainer(objectContainer)`

Closes the `objectContaienr`.

#### `objectContainerToPane(objectContainer)`

Moves `objectContainer` to a new pane. Returns the new pane.

#### `getEditorLayout(editor)`

Returns the current UI layout for the `editor`.

#### `expandLayout(editor, layout, callback)`

Expands a `layout` configuration inside `editor`. Callback receives the root `objectContainer` for the layout.

```js
paletteFunctions.expandLayout(editorPane, layout, function(objectContainer){
    console.log("Layout expanded:" + objectContainer)
})
```

#### `expandObjectContainerLayout(objectContainer, layout, options)`

Expands a `layout` configuration inside `objectContainer`. `options` are key value pairs for expanding options:

```js
var options = { expandChildren: true }
expandObjectContainerLayout(objectContainer, layout, options)
```