# QmlEditFragment

The `QmlEditFragment` contains an editable section of a qml document. It can have palettes as user interfaces to edit that particular section.

### Properties

#### `Object visualParent`

The visual parent of this fragment. This can be either an `ObjectContainer`, or a `PropertyContainer`, or a `PaletteGroup`, depending on the context.

#### `int refCount`

Contains the visual references of this fragment

#### `QmlEditFragment.Location location`

The location of this fragment: whether it's in a `property`, `object`, `imports`, `component` or `slot`.

#### `QmlEditFragment.Location valueLocation`

Some edit fragments are for properties that also have objects. In this case, `location` will be `QmlEditFragment.Property`, and
`valueLocation` will be `QmlEditFragment.Object`. In all other cases, the two properties will be the same.

#### `bool isWritable`

`true` if this fragment is writable.

#### `var whenBinding`

A function that will be called when the value from the application needs to be written in code. This value is set from a palette.

#### `LanguageQmlHandler language`

Returns the language handler for this fragment.

#### `bool isNull`

`true` if this fragment is null.

#### `int position()`

Returns the offset of this fragment in the document.

#### `int valuePosition()`

Returns the offset of the value for this fragment in the document.

#### `int valueLength()`

Returns the value length for this fragment in the document.

#### `int length()`

Returns the total length of this fragment whithin the document.

#### `string type()`

Returns the value type for this fragment.

#### `string typeName()`

Returns the value typeName for this fragment.

#### `string identifier()`

Returns the value identifier for this fragment. (i.e. for properites its `propertyName`)

#### `string objectInitializerType()`

Returns the value initializer type.

#### `string objectId()`

If the fragment wraps around an object, it returns its id.

#### `int fragmentType()`

Returns the fragment type, which can be a combination of the following:

 * `Builder`
 * `ReadOnly`
 * `Group`
 * `GroupChild`

#### `rebuild()`

Triggers a rebuild for the document of this fragment.

#### `list<string> bindingPath()`

Returns the binding path as a list of segments for this fragment.

#### `bool isMethod()`

Returns `true` if this fragment is a method.

#### `commit(var value)`

Commits the `value` of this fragment to the channel its connected to in the `application`.

#### `QtObject readObject()`

If this fragment wraps an object, it returns the object.

#### `QtObject propertyObject()`

If this fragment wraps a property object, it returns the object.

#### `string defaultValue()`

Returns the default value for this fragment, depending on its type.

#### `string readValueText()`

Reads the code set for the value of this fragment.

#### `object readValueConnection()`

Returns the code as a value connection object.

#### `var parse()`

Parses this fragment and returns the value.

#### `bool bindExpression(string expression)`

Creates a binding with `expression`, returns `true` on success.

#### `bool bindFunctionExpression(string expression)`

Creates a function binding with `expression`, returns `true` on success.

#### `writeProperties(object properties)`

If this fragment is for an object, it writes all the `properties` to code.

#### `write(var value)`

Converts `value` to code, and writes it to the value section of this fragment.

#### `writeCode(string code)`

Writes `code` to the value section of this fragment.

#### `int totalPalettes()`

Returns the total number of palettes opened by this fragment.

#### `list<CodePalette> paletteList()`

Returns the list of palettes opened by this fragment.

#### `CodePalette bindingPalette()`

Returns binding palette if there is any set.

#### `updateBindings()`

Updates binding values for this fragment.

#### `QmlEditFragment parentFragment()`

Returns the parent fragment of this fragment, or null if there isn't any.

#### `list<QmlEditFragment> getChildFragments()`

Returns the child fragments for this fragment.

#### `removeChildFragment(QmlEditFragment edit)`

Removes the child `edit` fragment from this fragment.

#### `incrementRefCount()`

Increments the number of UI references for this fragment.

#### `decrementRefCount()`

Decrements the number of UI references for this fragment.

#### `suggestionsForExpression(string expression, CodeCompletionModel model, bool suggestFunctions)`

Adds suggestions for `expression` inside `model` extracted from whithin the document and context of this fragment.

#### `signalChildAdded(QmlEditFragment ef, object context = {})`

Triggers the `childAdded` signal for this fragment. The context is an object that describes the context in which the child was added.