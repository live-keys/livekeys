# LanguageQmlHandler

LanguageQmlHandler provides the core functions that work with qml documents. From parsing to creating QmlEditFragments and
providing channels of communication between them and the runtime.

### Properties

#### `CodeHandler code`

Reference to the parent code handler.

#### `QmlEditFragmentContainer editContainer`

Reference to the `QmlEditFragmentContainer`, which contains all the fragments.

#### `DocumentQmlChannels bindingChannels`

Stores the document binding channels, which contains the connection channels to the application.

#### `QmlEditFragment importsFragment`

Reference to the imports fragment if opened.

#### `QmlEditFragment rootFragment`

Reference to the root fragment if opened.

### Functions

#### `onDocumentParsed(function callback)`

Register a handler when the document finished parsing.

```js
languageHandler.onDocumentParsed(function(){
    console.log("Document parsed")
})
```

#### `onImportsScanned(function callback)`

Register a handler when the document finishes scanning its imports.

#### `list<int> languageFeatures()`

Returns a list of available features this language handler can provide for the qml language.

#### `string help(int position)`

Finds whether there's a declaration at `position` that could have documentation set up, and returns the identifier for that documentation, which can be looked for using certain rules.

#### `toggleComment(int position, int length)`

Toggles comment for lines between `position` and `position + length`

#### `QmlEditFragment openConnection(int position)`

Opens the connection for the declaration at `position`

#### `QmlEditFragment openNestedConnection(QmlEditFragment edit, int position)`

Opens a child connection at `edit` for the declaration at `position`.

#### `QmlEditFragment openReadOnlyPropertyConnection(QmlEditFragment parentFragment, string name)`

Opens a read only connection at `parentFragment`, for the property `name`.

#### `list<QtObject> openNestedFragments(QmlEditFragment edit, object options = {})`

Opens all the nested fragments from `edit`. Options is an array to limit the type of nested fragments to open:

```js
var allConnections = languageHandler.openNestedFragments(edit)
var onlyPropertyConnections = languageHandler.openNestedFragments(edit, ['properties'])
var onlyObjectConnections = languageHandler.openNestedFragments(edit, ['objects'])
```

#### `list<QmlEditFragment> editFragments()`

Returns all the root editing fragments in the documnet. Root editing fragments are fragments that are managed by the `LanguageQmlHandler`, and do not have any parent fragments.

#### `removeConnection(QmlEditFragment edit)`

Removes the `edit` fragment from the document.

#### `eraseObject(QmlEditFragment edit, bool removeFragment = true)`

If `edit` is of object type, it will erase the object within the application, and by default will remove the fragment as well.

#### `object findPalettesForFragment(QmlEditFragment fragment)`

Find all the palettes that can be opened inside `fragment`.

Returns an object with the list of palettes.

```js
var result = languageHandler.findPalettesForFragment(edit)
```

Result will have the following structure:

```js
{
    data: [...], // array of palette names
    declaration: {...}, //declaration object, see declarationInfo(position)
    defaultPalette: '' // a configured default palette if any
}

```

#### `object findPalettes(int position)`

Find all the palettes that can be opened at `position`. 

Returns an object with a declaration and palettes. See `findPalettesForFragment`

#### `QmlEditFragment removePalette(CodePalette palette)`

Close and remove `palette` from fragment.

#### `CodePalette openBinding(QmlEditFragment edit, string paletteName)`

Open binding in `edit` fragment through `paletteName`.

#### `void closeBinding(int position, int length)`

Close bindings between `position` and `position + length`.

#### `object expand(QmlEditFragment edit, object layout)`

Expand `edit` with `layout` options.

#### `object declarationInfo(int position, int length)`

Returns declaration information at `position`. If length is specified, and there are multiple declarations found between `position` and `position + length`, then an array of declarations is returned.

A declaration info object will have the following structure:

```js
{
    position: 0, // declaration position
    length: 0, // declaration length
    type: 'qml/lcvcore#Image', // declaration type
    parentType: '', // parent type if any
    location: 'property', // can be property/component/imports/list/slot
    hasObject: false // if the declaration is for a property, does it have an object declared
}
```

#### `object contextBlockRange(int cursorPosition)`

Returns an object describing the range of the block wrapping `cursorPosition`:

```js
var range = languageHandler.contextBlockRange(100)
console.log(range.begin)
console.log(range.end)
```

#### `Array getDocumentIds()`

Return all the ids within this document.

#### `QmlMetaTypeInfo typeInfo(string fullTypeName)`

Returns the type info for a specified type that is available in this document:

```js
var typeInfo = languageHandler.typeInfo('qml/lcvcore#ImageFile')
```

#### `QmlImportsModel importsModel()`

Returns the imports model for this document, containing the list of declared imports.

#### `int findImportsPosition()`

Returns the offset of the imports within the document.

#### `int findRootPosition()`

Returns the offset of the root within the document.

#### `suggestCompletion(int cursorPosition)`

Trigger the completion model at `cursorPosition`

#### `CodePalette *edit(QmlEditFragment ef)`

Create an edit fragment at the connection specified by `ef`

#### `void cancelEdit()`

Cancel the edit operation.

#### `QmlAddContainer getAddOptions(object value)`

Returns the a model describing the available entries to add depending on `value`.

`value` can either specify a `QmlEditFragment`:

```js
var addModel = languageHandler.getAddOptions({editFragment: ef, isReadOnly: false})
```

Or a position:

```js
var addModel = languageHandler.getAddOptions({position: 100})
```

#### `object addPropertyToCode(int position, string name, string value, QmlEditFragment parentGroup = null)`

Adds property `name` to code, with the given `value`, around `position`. `position` is taken as recommended position, as the handler will try to find the best position to insert the property. `parentGroup` is used for readOnly properties, i.e. (`anchors.left`)

Returns an object containing the number of chars added (`totalCharsAdded`) and the actual postion the property was added at.

If the property already exists in code for the given object, the position will be returned, and the number of chars added will be `0`.

#### `int addObjectToCode(int position, var type, object properties = {})`

Adds an object of `type` to code.  The `type` can be specified in multiple ways:

 * As a string, describing the type as it would be declared in the document: `ImageFile`
 * As a string, describing the type as a global reference: `qml/lcvcore#ImageFile`
 * As an object, with `type` and `id` properties, if the user wants to add an id to the type: `{ type: 'qml/lcvcore#ImageFile', id: imageFile }`

 `extraProperties` is optional, in case the user wants to add extra properties to the newly created object. The `extraProperties` is an array of objects, where
each object has can have a `name`, `type` and `value`. The `value` is optional, as the value will be assigned with a default value if there's nothing else specified.

`position` is taken as a recommended position, as the handler will try to find the best position to insert the object.

Returns the actual postion the object was added at.

#### `int addEventToCode(int position, string name)`

Adds event `name` to code around `position`. `position` is taken as a recommended position, as the handler will try to find the best position to insert the object.

Returns the actual postion the event was added at.

#### `int addRootObjectToCode(var type)`

Adds a root object of `type` to code. The `type` can be specified in multiple ways:

 * As a string, describing the type as it would be declared in the document: `ImageFile`
 * As a string, describing the type as a global reference: `qml/lcvcore#ImageFile`
 * As an object, with `type` and `id` properties, if the user wants to add an id to the type: `{ type: 'qml/lcvcore#ImageFile', id: imageFile }`

 Returns the position the root was added at.

#### `createObjectForProperty(QmlEditFragment propertyFragment)`

Creates the object for a `propertyFragment`, by using the type of the `propertyFragment`.

#### `createObjectInRuntime(QmlEditFragment edit, string type = "", object properties = {})`

Creates the object of `type` in runtime. The `type` can be specified in multiple ways:

 * As a string, describing the type as it would be declared in the document: `ImageFile`
 * As a string, describing the type as a global reference: `qml/lcvcore#ImageFile`
 * As an object, with `type` and `id` properties, if the user wants to add an id to the type: `{ type: 'qml/lcvcore#ImageFile', id: imageFile }`
