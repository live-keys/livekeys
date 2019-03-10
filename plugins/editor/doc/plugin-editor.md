# Plugin 'editor'

This module contains all qml types related to the editor used in LiveKeys

```
include editor 1.0
```

{plugin:editor}
{qmlSummary:editor}

{qmlType:Editor}
{qmlInherits:external.QtQuick#Rectangle}
{qmlBrief:Visual item that displays the text editor}

This type is used extensively inside LiveKeys for each document and fragment opened.

{qmlProperty:bool isDirty}

Indicates that the contained document is dirty

{qmlProperty:string text}

Text contained inside the editor 

{qmlProperty:var font}

Font of the text

{qmlProperty:bool internalActiveFocus}

Indicates that the editor has active focus

{qmlProperty:bool internalFocus}

Indicates that the editor has focus

{qmlProperty:var documentHandler}

Document handler of the currently opened document

{qmlProperty:NewTextEdit textEdit}

Text edit that is used to display and manipulate the document. The main object of the editor


{qmlProperty:var windowControls}

A set of globally available variables

{qmlProperty:var document}

The document opened inside the editor

{qmlProperty:int fragmentStart}

Start line of a fragment, in case only a fragment of the document is visible

{qmlProperty:int fragmentEnd}

End line of a fragment, in case only a fragment of the document is visible

{qmlProperty:color topColor}

Color of the editor menu

{qmlMethod:hasActiveEditor()}

Indicates if LiveKeys has an active editor

{qmlMethod:save()}

Save the current document

{qmlMethod:saveAs()}

Open the `Save as` dialog

{qmlMethod:closeDocument()}

Close the current document (save in case saving is necessary)

{qmlMethod:var getCursorFragment()}

Return the fragment boundaries for the fragment that contains the cursor

{qmlMethod:closeDocumentAction()}

The complete action for closing the document (in case it's the only document in the project, potentially closes the project too)

{qmlMethod:Rectangle getCursorRectangle()}

Returns the cursor rectangle 

{qmlMethod:assistCompletion()}

Assist completion at current cursor position

{qmlMethod:toggleSize()}

Change size of editor to be either 3/4, 1/2 or 1/4 or the parent

{qmlMethod:forceFocus()}

Force the focus on the editor

{qmlMethod:var cursorWindowCoords()}

Returns the coordinates of the cursor

{qmlType:CodePalette}
{qmlInherits:external.QtCore#Object}
{qmlBrief:Palette display of a property}

Palettes are useful visual tools for editing the values of some properties such as ints, doubles, colors etc.
They can be easily opened by right-clicking on the property, and selecting "Palette" or "Shape" options, or using a related shortcut.

{qmlProperty:Item item}

Visual item for this palette

{qmlProperty: string name}

Palette name

{qmlProperty:string type}

Palette type

{qmlProperty:var value}

Property value of the palette (could be a variety of types)

{qmlProperty:object extension}

Extension for this palette

{qmlSignal:init(var value)}

Signals that the value was initialized

{qmlType:LiveExtension}
{qmlInherits:external.QtCore#Object}
{qmlBrief:The class within Qml to be used when we want to add an extension}

An example of an extension is the locally used **EditQmlExtension.qml**.

Each of the properties below has an appropriate signal `<propertyName>Changed()` to indicate change.

{qmlProperty:object globals}

Globally available variables inside LiveKeys

{qmlProperty:var interceptLanguage}

Function that returns a handler for the given extension (e.g. qml, js)

{qmlProperty:var interceptMenu}

Function that returns a menu array for the given extension (e.g. qml)

{qmlProperty:var commands}

Object of commands for the given extension

{qmlProperty:var keyBindings}

Key shortcuts for the given extension

{qmlProperty:var configuration}

Extension configuration

{qmlType:DocumentHandler}
{qmlInherits:external.QtCore#Object, external.QtQml#ParserStatus}
{qmlBrief:The go-to class when it comes to handling documents}

Forwards everything to the highlighter, has a completion model in case there's a specific code handler attached to it,
it can auto-complete code, which is all behavior inherited from the AbstractCodeHandler.

{qmlProperty:var completionModel}

Model used for completion of code

{qmlProperty:var codeHandler}

Handler for the document code

{qmlProperty:bool editorFocus}

Indicator that the editor is in focus

{qmlMethod:insertCompletion(int from, int to, string completion)}

Adds specific completion that the user picked between given positions

{qmlMethod:documentContentsChanged(int position, int charsRemoved, int charsAdded)}

Slot that is connected to document changes

{qmlMethod:cursorWritePositionChanged(var cursor)}

Slot reacting to cursor position change
Potentially triggers the assisted completion

{qmlMethod:setDocument(var document, var options)}

Document that the document handler is operating on

{qmlMethod:documentFormatUpdate(int position, int length)}

Slot for changes in document format - triggers a rehighlight

{qmlMethod:generateCompletion(int cursorPosition)}

Generates code completion at a given cursor position

{qmlMethod:contextBlockRange(int cursorPosition)}

Finds the boundaries of the code block containing the cursor position

{qmlMethod:manageIndent(int from, int length, bool undo)}

Used to manage indentation of selected text

{qmlSignal:targetChanged()}

Target document changed

{qmlSignal:cursorPositionRequest(int position)}

Cursor position change request

{qmlSignal:contentsChangedManually()}

Document content was changed manually

{qmlSignal:editorFocusChanged()}

Editor focus changed

{qmlSignal:codeHandlerChanged()}

Code handler changed
