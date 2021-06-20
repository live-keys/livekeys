# Workspace Layer Panes

Panes are the main visual items that describe the layout of a workspace. Panes
are configurable by the user, meaning they can be dragged, resized, split, and
removed depending on the users preferences.

Panes can be of different types:

 * An **editor** pane contains the code editor
 * A **project file system** pane shows the files in the given project
 * A **log** pane contains the log outputs of the application
 * A **view** pane contains the application output

Some pane types are *single* or *singletons*, meaning there can be only one opened. For example, the main **view** pane, and the **project file system** pane can only be opened once. On the other hand, multiple **editors** can exist side by side.

The type of panes available can be extended to custom panes, which will be covered a bit later in this page.

Depending on the type, some panes have a `state`. A `state` is a property describing the current state of the pane. For example, an **editor** pane will have the opened document as part of it's state. This is useful when pane layouts are restored for different projects. Each pane will initialize with the state it was last seen in.

Panes can also be distributed throghout multiple windows. Users can open multiple Livekeys windows, and drag and 
drop panes depending on what they want to view on each side.

The pane API handles the management of these panes. The API can be accessed through:

```js
lk.layers.workspace.panes
```

## Viewing available pane types

#### ```paneTypes()```

Returns a list of available pane types:

```js
var paneTypes = lk.layers.workspace.panes.paneTypes()
// paneTypes == [{name: 'editor', single: false}, {name: 'projectFileSystem', single: false}, ...]
```

## Creating panes

The `createPane` function is used to create panes:

```js
var editorPane = lk.layers.workspace.panes.createPane('editor')
```

The pane state and size can also be sent as arguments to this function:

```js
var editorPane = lk.layers.workspace.panes.createPane('editor', {document: '<path_to_doc>'}, [800, 600])
```

This function will only create a pane, but won't add it to the pane layout. This means the pane will be invisible, and it won't be usable from the view.

## Adding a pane to the layout

<Panes are nested inside Split views. Split views align the panes horizontally or vertically, depending on their type>
<If a pane is added to the layout, the SplitView a pane is nested in can be acccessed via `pane.parentSplitView`>
<The index of a pane inside a splitview can be acceseded through `pane.parentSplitViewIndex`>

When adding a pane to the layout, other panes need to be resized in order to make room for the newly added pane.
Depending on the resize method, there are 2 types of functions available:

### Functions that split panes

#### ```splitPaneHorizontally(currentPane, newPane)```

This function will split a pane horizontally, reducing it's size to half, and inserting the new pane in the other half.

#### ```splitPaneVertically(currentPane, newPane)```

This function will split a pane vertically, reducing it's size to half, and inserting the new pane in the other half.

#### ```splitPaneHorizontallyBefore(currentPane, newPane)```

This function will split a pane horizontally, reducing it's size to half and moving it to the right, and inserting the new pane in the other half.

#### ```splitPaneVerticallyBefore(currentPane, newPane)```

This function will split a pane vertically, reducing it's size to half and moving it to the bottom, and inserting the new pane in the other half.

For all these functions, if the pane size will be smaller than the minimum size of the pane being split, 
the panes following it will be resized in order to fit the new pane. If that is not possible, an error 
will be thrown.

### Functions that insert panes

Panes can also be inserted at a specified index in a `SplitView`.

#### ```insertPaneAtIndex(splitView, index, newPane)```

The panes within that splitView are resized proportionally, so the new pane can fit in. If the panes cannot fit the new  

#### ```insertPane(newPane, options)```

Tries to find a position to insert the pane at.

The preferred orientation can be specified:

```js
lk.layers.workspace.panes.insertPane(pane, { orientation: Qt.Horizontal } )
```

### Checking if pane can fit

#### ```canFitPane(splitView, pane)```

Returns true if the `splitView` has enough size available to fit `pane`, false otherwise. This function takes into account the minimum sizes of all the panes inside the `splitView`.

##  Searching for panes

#### ```focusPane(paneType)```

If the pane in focus fits the `paneType`, the pane in focus is returned. Otherwise, the first pane that fits
the `paneType` is returned.

```js
var pane = lk.layers.workspace.panes.focusPane('editor')
if ( pane && pane === lk.layers.workspace.panes.activePane )
    console.log('Found pane in focus')
```

#### ```findPanesByType(paneType)```

Returns all the panes of `paneType`:

```js
var panes = lk.layers.workspace.panes.findPaneByType('editor')
console.log("Total panes found: " + panes.length)
```

## Active panes and items.

A single pane together with an item inside a pane can be active at the same time. For example, the `TextEdit` inside the `Editor` pane is the item actually in focus, but the active pane is the `Editor` pane.

There are 2 properties to get the active item and active pane.

#### ```activeItem```

#### ```activePane```

#### ```activateItem(item, pane)```

This function activates the `item` inside `pane` and sets it in focus.

#### ```setActiveItem(item, pane)```

Sets the active `item` and `pane`. The focus needs to be set manually.

## Removing panes from the layout

#### ```removePane(pane)```

This function removes the pane from the layout, without destroying it. The pane should be cleared manually
when not used anymore using `clearPane`.

#### ```clearPane(pane)`

This function removes the pane from the layout if it's there, and clears it, making it unusable. Use this if
you don't want to reuse the pane.

#### ```reset()```

Resets the entire layout, clearing all panes.

## Moving panes to new windows

#### ```movePaneToNewWindow(pane)```

Moves `pane` to a new window.

## Extending pane types

The type of panes available can be extended to custom panes, by adding a package with a *workspace extensions*. An extension can add a pane type by configuring the `panes` property:

 ```js
WorkspaceExtension{
    id: root

    property Component myPane : Component{
        MyPane{}
    }

    panes: {
        "myPane" : {
            create: function(p, s){
                var pane = root.myPane.createObject(p)
                if ( s )
                    pane.paneInitialize(s)
                return pane
            },
            single: false
        }
    }
}
```

The `panes` property can include multiple panes through key-value pairs. The keys represent the unique name
the pane will be created from, and on the value side there are 2 properties. One is the `create` function, which
returns the created pane, and the `single` property, which describes whether the pane is a singleton or not.


The custom created pane is a `Pane` component:

*MyPane.qml*

```js
Pane{
    paneType: 'myPane'
    paneState : { return {} }
    paneInitialize : function(s){
        console.log("Initializing state")
    }
    paneCleared: function(){
        console.log("Pane cleared")
    }
}
```

 * `paneType` describes the pane type
 * `paneState` is the state of the pane, which will be saved in the layout file, and can be retrieved if the same pane is recreated
 * `paneInitialize` is a function that will be called when the pane initializes from the layout file, given `s` as its state
 * `paneCleared` gets triggered when the pane gets removed and cleared from the layout.

