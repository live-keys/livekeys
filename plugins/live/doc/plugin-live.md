# Plugin 'live'

This modules contains the main qml files that link to LiveKeys functionality.

```js
import live 1.0
```

{plugin:live}
{qmlSummary:live}

{qmlType:Main}
{qmlInherits:external.QtQuick#Item}
{qmlBrief:Provides a main script entry to a qml application.}

An example of use can be found in **samples/live/mainscript.qml**

{qmlProperty:array options}

Configuration options to parse received arguments. There are two types of options. Value types and flag
types. Flags are simply checked to see whether they have been set within Live CV.

```qml
Main{
  id: main
  options : [
	{ key: ['-f', '--someflag'], describe: 'Enables a feature.' }
  ]
}
```

Then we can check if the option has been set by querying our Main object:

```js
console.log('Flag status: ' + main.isOptionSet('-f'))
```

Value types need an extra `type` key and an optional `required` key, which by default is false:

```qml
Main{
  id: main
  options : [
	{ key: ['-v', '--value'], type: 'string', describe: 'Receive value.', required: true }
  ]
}
```

The type can be anything used to intuitively describe the value. It does not have to match a qml type.
If an option is required and is missing when Live CV is called, then Live CV will exit, and a help text will be
displayed to the user.

{qmlProperty:string version}

Sets the script version.

{qmlSignal:run()}

Emited once the arguments are parsed and the script is ready to run.

{qmlMethod:list arguments()}

Returns the arguments after extracting the parsed options

{qmlMethod:string option(string key)}

Returns the option configured at `key`. Raises a warning if the option was not configured.

{qmlMethod:bool isOptionSet(string key)}

Returns true if the option at `key` has been set. False otherwise.

{qmlType:Triangle}
{qmlInherits:external.QtQuick#Item}
{qmlBrief:A simple visual representation of an oriented triangle}

An example of use:
```qml
Triangle{
	...
	width: 9
	height: 5
	color: "#9b6804"
	rotation: Triangle.Bottom
}
```

{qmlEnum:Rotation}

Orientation of the triangle can be one of the following:
* Top
* Right
* Bottom
* Left

{qmlProperty:Triangle.Rotation rotation}

Represents the rotation of the triangle i.e. whether it's pointing up, down, left or right.

{qmlProperty:Color color}

Returns the color of the triangle.

{qmlType:StaticLoader}
{qmlInherits:external.QtQuick#Item}
{qmlBrief:Creates and stores a component statically. See {Static Items} for more information on statics in Live CV.}

An example is available at **samples/live/staticloader.qml**

{qmlProperty:Component source}

Component to load statically.

{qmlProperty:object item}

This property holds the top-level object that was loaded

{qmlSignal:itemCreated()}

This signal is emitted when the item is created. This is usually at the start of the application, or when
the user calls the staticLoad function with a different state id than the previous compilation.

{qmlMethod:staticLoad(string id)}

Loads the StaticLoader state. `id` has to be unique for this component when used in context with other StaticLoaders.

{qmlType:FileReader}
{qmlInherits:external.QtCore#Object, external.QtQml#ParserStatus}
{qmlBrief:Represents a file reader which extracts the data from a file and potentially monitors changes.}

See a simple example at **samples/live/filereading.qml**

{qmlProperty:string source}

Represents the path to our file. 

{qmlProperty:bool monitor}

Indicates if the (external) file changes should be monitored.

{qmlProperty:array data}

Represents the data read from the file.

{qmlMethod:string asString(array data)}

Return the loaded data as a string

{qmlMethod:systemFileChanged(string file)}

React to a change inside a file by resyncing data.

{qmlType:StaticFileReader}
{qmlInherits:external.QtQuick#Item}
{qmlBrief:Loads a given file statically}

An example cna be seen at **samples/live/staticfilereading.qml**

{qmlProperty:array data}

The data extracted from the file

{qmlMethod:staticLoad(string file, object params)}

Loads the given file statically, with the params potentially containing only a single property, "monitor", in order to monitor external changes.

{qmlMethod:string asString(array data)}

Returns the loaded data as a string

{qmlMethod:readerDataChanged(array data)}

Reacts to data changes inside the file by resyncing local copy of the data.

{qmlSignal:init()}

Called when the loader is initialized (component complete).

{qmlType:VisualLogFilter}
{qmlInherits:base#VisualLogBaseModel}
{qmlBrief:Represents a filtered set of log entries}

The filter can be applied via tag, prefix or a regular search string

An example can be found in **samples/live/visuallogfilter.qml**

{qmlProperty:VisualLogBaseModel source}

Base model which is the model we're applying the filter too.

Interestingly, it can be both the main model, or even another filter model!
We could, in theory, have an array or filter models where each one is filtering on the previous one.

{qmlProperty:string tag}

The tag string that we're filtering by

{qmlProperty:var prefix}

The prefix string/regexp that we're filtering by 

{qmlProperty:var search}

The string/regexp we're filtering log messages by

{qmlProperty:bool isIndexing}

Indicator that the search is being done within the source model

{qmlMethod:refilterReady()}

Slot that listens for the ending of indexing in the background.

This slot gets called after the worker finishes, but also gets called only after
control is returned to the event loop. Any changes in between will be lost, therefore
we handle the changes separately, using the m_workerIgnoreResult variable.

{qmlMethod:sourceDestroyed()}

Source is destroyed


{qmlMethod:sourceModelReset()}

Reacts to changes in the source model

{qmlMethod:sourceModelAboutToReset()}

Before a reset, we ignore the results of the worker because they're not valid anymore

{qmlMethod:sourceRowsAboutToBeRemoved(ModelIndex, int from, int to)}

When source model is having rows removed, we ignore the worker results and rebuild

{qmlMethod:sourceRowsInserted(ModelIndex, int from, int to)}

When source model is having rows added, we ignore the worker results and rebuild

{qmlType:StringBasedLoader}
{qmlInherits:external.QtQuick#Item}
{qmlBrief:Given a piece of qml code as string, it will create the item}

An example is available at **samples/live/stringbasedloader.qml**

{qmlProperty:string source}

The source code of the item

{qmlProperty:object item}

The created item

{qmlType:ComponentSource}
{qmlInherits:external.QtQml#QtObject}
{qmlBrief:Maps its internal source component to a string, together with the imports within the file it was declared on. Can be used to map components that should be created in different processes.}

{qmlProperty:Component source}

The item whose source code will be copied.

{qmlType:TextButton}
{qmlInherits:external.QtQuick#Rectangle}
{qmlBrief:Represents a button with a custom text}

An example can be found at **samples/live/valuehistory.qml**

{qmlProperty:string text}

Button display text

{qmlProperty:string fontFamily}

Font family

{qmlProperty:int fontPixelSize}

Font size

{qmlProperty:color textColor}

Color of the button text

{qmlProperty:color backgroundHoverColor}

Color of the button when we're hovering over it

{qmlProperty:color backgroundColor}

Color of the button

{qmlSignal: clicked()}

Signal that we've clicked the button

{qmlType:VisualLogView}
{qmlInherits:external.QtQuick.Controls#ScrollView} 
{qmlBrief:A view to display the messages of a visual log filter}

An example can be seen in **samples/live/visuallogfilter.qml**

{qmlProperty:var model}

Model to be displayed 

{qmlProperty:color backgroundColor}

Background color of the view

{qmlSignal:itemAdded()}

Notifies that a log message has been added
