# Plugin `base`

This module contains basic structures that should be available throughout LiveKeys.

```js
import base 1.0
```

{plugin:base}
{qmlSummary:base}

{qmlType:VisualLog}
{qmlBrief:Main logging object}

For the concepts behind logging in LiveKeys, check out the Logging page.

Example of logging:
```qml
	vlog.d("Hello world!")
```



{qmlType:VisualLogQmlObject}
{qmlInherits:QObject}

Contains methods to present a Qml output for the visual log.

## Summary

 Members                        | Descriptions                                
--------------------------------|---------------------------------------------
`public  `[`VisualLogQmlObject`](#classlv_1_1VisualLogQmlObject_1ac3a27d9bb38cd9f189c95f8a79d61257)`(QObject * parent)` | Default constructor
`public  `[`~VisualLogQmlObject`](#classlv_1_1VisualLogQmlObject_1a54f3f38b1658241041748e82f8f1deb3)`()` | Default destructor
`{slot} public void `[`f`](#classlv_1_1VisualLogQmlObject_1a2b7123f619d3564da11bdc75e99f61ea)`(const QJSValue & messageOrCategory,const QJSValue & message)` | Fatal messages
`{slot} public void `[`e`](#classlv_1_1VisualLogQmlObject_1a5d4f19af2f90eb47048c733d7146b347)`(const QJSValue & messageOrCategory,const QJSValue & message)` | Error messages
`{slot} public void `[`w`](#classlv_1_1VisualLogQmlObject_1a76fbd7794d720dfe64178358eb63b2da)`(const QJSValue & messageOrCategory,const QJSValue & message)` | Warning messages
`{slot} public void `[`i`](#classlv_1_1VisualLogQmlObject_1abeb16144544ca7f470473d723ba429aa)`(const QJSValue & messageOrCategory,const QJSValue & message)` | Info messages
`{slot} public void `[`d`](#classlv_1_1VisualLogQmlObject_1a1051706a172dc9dfd397df4dce67aed6)`(const QJSValue & messageOrCategory,const QJSValue & message)` | Debug messages
`{slot} public void `[`v`](#classlv_1_1VisualLogQmlObject_1a7dafe4fb50af4f244deb3309f8215630)`(const QJSValue & messageOrCategory,const QJSValue & message)` | Verbose messages
`{slot} public void `[`configure`](#classlv_1_1VisualLogQmlObject_1ae75b3b3484c2711a6c95772b745f5fed)`(const QString & name,const QJSValue & options)` | Configures global vlog object from a given QJSValue object


{qmlType:LiveScript}
{qmlBrief:A wrapper class for running a script, containing command arguments and LiveKeys environment data}

It's available throughout QML in LiveKeys through the `script` property. 

{qmlProperty:list argv}

List of all arguments given as strings, including the script name.

{qmlProperty:list argvList}

Potentially empty list of additional arguments for the script. Same as `argv` sans the script name.

{qmlProperty:LiveEnvironment environment}

Collection of enviroment data.

{qmlMethod:void scriptChanged(ProjectDocument active)}

Reacts to changes to the active document, and changes the script name.

{qmlMethod:string name()}

Returns the name of the script.

{qmlType:Environment}
{qmlBrief:Container for parameters of the environment LiveKeys is running in.}

It's available throughout QML in LiveKeys through the `script.environment` property. 

{qmlProperty:QVariantMap os}

Represents a map of values related to the operating system LiveKeys is running on.

{qmlType:ObjectList}
{qmlInherits:base#Shared}
{qmlBrief:Contains a custom list of objects with a user buffer}

{qmlProperty:QQmlListProperty items}

List of all item objects contained within

{qmlProperty:ObjectListModel model}

Model of this data to be used within Qml

{qmlMethod:QObject itemAt(int index)}

Function that gets us the item at the given position.

{qmlMethod:int itemCount()}

Number of objects the list contains

{qmlMethod:void clearItems()}

Removes all elements from the container

{qmlMethod:void appendItem(QObject item)}

Adds item at the end of the container

{qmlMethod:void removeItemAt(int index)}

Removes item at given index

{qmlMethod:ObjectListModel model()}

Returns model 

{qmlMethod:ObjectList cloneConst()}

Creates a read-only clone of the container

{qmlMethod:ObjectList clone()}

Creates a clone of the container

{qmlType:ObjectListModel}
{qmlInherits:external.QtModels#AbstractListModel}
{qmlBrief:Model that provides us access to the ObjectList}

{qmlMethod:appendData(Object data)}

Appends a given object to the list

{qmlMethod:removeAt(int index)}

Removes the object at the given index from the list

{qmlMethod:Object at(int index)}

Returns the object at the given index

{qmlMethod:int size()}

Returns the number of items


{qmlType:VariantList}
{qmlInherits:base#Shared}
{qmlBrief:Contains a custom list of values with a user buffer}

{qmlProperty:QVariantList items}

List of all values contained within

{qmlMethod:list items()}

Returns a list of items

{qmlMethod:var itemAt(int index)}

Returns the item at given index

{qmlMethod:int itemCount()}

Returns the count of items

{qmlMethod:bool isConst()}

Shows if the list is immutable

{qmlMethod:void clearItems()}

Clear all items

{qmlMethod:void appendItem(var item)}

Appends the given item to the end of the list

{qmlMethod:void removeItemAt(int index)}

Remove item at the given index

{qmlMethod:VariantListModel model()}

Returns the model

{qmlMethod:VariantList cloneConst()}

Create an immutable clone of the list

{qmlMethod:QmlVariantList* clone() const}

Create a clone of the list

{qmlType:VariantListModel}
{qmlInherits:external.QtModels#AbstractListModel}
{qmlBrief:Model that provides us access to the VariantList}

{qmlMethod:appendData(var data)}

Appends a given value to the list

{qmlMethod:removeAt(int index)}

Removes the value at the given index from the list

{qmlMethod:var at(int index)}

Returns the value at the given index

{qmlMethod:int size()}

Returns the number of items