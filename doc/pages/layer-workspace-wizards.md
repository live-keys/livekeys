# Workspace Layer Wizards

Wizards are predefined user interactions that go through different checks when
working with the project.

For example, a file in the project can be closed using the workspace project object:

```js
lk.layers.workspace.project.closeFile('/path/to/file')
```

This will however, close the file without saving, and without checking with the user first.
By using a wizard, we avoid this problem, and let Livekeys deal with all the checks:

```js
lk.layers.workspace.wizards.closeFile('/path/to/file')
```

This will prompt the user to save the file if the file has any unsaved changes.


The following wizards are available:

#### `openProjectDirViaDialog(callback)`

Opens a new project directory through a dialog box, allowing the user to choose a path. If a project is opened before, it will close it via the `closeProject()` function.

```js
lk.layers.workspace.wizards.openProjectDirViaDialog(function(path){
    console.log('Project opened:' + path)
})
```

#### `openProjectFileViaDialog(callback)

Opens a new project file through a dialog box, allowing the user to choose a path. If a project is opened before, it will close it via the `closeProject()` function.

```js
lk.layers.workspace.wizards.openProjectFileViaDialog(function(path){
    console.log('Project opened:' + path)
})
```

#### `openProject(url, callback)`

Opens a project at `path`. If there's a project already opened, it will close
it before.

```js
lk.layers.workspace.wizards.openProject('/path/to/project', function(path){
    console.log('Project opened:' + path)
})
```

#### `newProject(callback)`

Creates a new project. If there's a project already opened, it will close
it before.


```js
lk.layers.workspace.wizards.newProject(function(){
    console.log('Project created.')
})
```


#### `closeProject(callback)`

Closes the current project, checking for any unsaved files, and asking the user for confirmation:

```js
lk.layers.workspace.wizards.closeProject(function(){
    console.log('Project closed')
})
```


#### `openFileViaDialog(callback)`

Opens a new file by opening a dialog box for the user to choose one. Depending on
the extensions installed, the file might be opened as a TextDocument, or a simple
Document. A pane will be initialised as well if needed.

```js
lk.layers.workspace.wizards.openFileViaDialog(function(path, document, pane){
    console.log('File opened: ' + path, document, pane)
})
```

#### `openFile(path, mode, callback)`

Opens a new file at the given `path`. The mode can be `Document.Edit`,
`Document.Monitor` or `Document.EditIfNotOpen`.

```js
lk.layers.workspace.project.openFile('/path/to/file', Document.Edit, function(path, document, pane){
    console.log('File opened: ' + path, document, pane)
})
```

#### `closeFile(path, callback)`

Closes a file at the given `path` if there's a file opened. Will perform a check
to save the file if the file has been changed since the last save.

```js
lk.layers.workspace.wizards.closeFile('/path/to/file', function(){
    console.log('File closed.')
})
```

#### `addFile(path, opt,  callback)`

Queries the user for a file name, after which it will create it at the given `path`. `path` must be inside the project directory.
Options can be one of the following:
    * heading: specific heading shown in the add box
    * suggestion: any suggestion on the name of the file
    * extension: extension to be set when adding the file.

```js
lk.layers.workspace.wizards.addFile('/path/to/path/in/project', {}, function(path){
    console.log('File added: ' + path)
})
```

#### `addDirectory(path, callback)`

Queries the user for a directory name, after which it will create it at the given `path`. `path` must be inside the project directory.

```js
lk.layers.workspace.wizards.addDirectory('/path/to/path/in/project', function(path){
    console.log('Directory added: ' + path)
})
```

#### `checkUnsavedFiles(callback)`

Performs a check and asks the user what he wants to do with all the unsaved files.

```js
lk.layers.workspace.wizards.checkUnsavedFiles(function(){
    console.log('Check finished.')
})
```
