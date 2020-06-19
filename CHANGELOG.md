# ChangeLog

This file summarises notable changes between Live Keys versions.

## 1.7.0

### Features

   - Added node editor.
   - Added palettes in node editor.
   - Added timeline editor.
   - Added interactive documentation.
   - Added workspace package. (`import workspace`)
   - Palettes on imports.
   - Added shape-all option.
   - Added pane for palette object containers.
   - Support for palettes outside main file.
   - Connection palette

## 1.6.1

### Bug Fixes

    - Fixed code completion in some scenarios where the context was off
    - Cursor position is now remembered when switching documents
    - Fixed editor scrolling during some indenting scenarios

## 1.6.0 - Rename to Livekeys

### Support

   - Updated to Open CV 4

### Features

   - Layer based loading (Current layers: base, window, workspace, editor)).
   - Support for a FileSystem package. (```import fs```)
   - Added workspace pane manager.
   - Added workspace themes.
   - Added support for packages and dependencies. (Packages are also found in the *packages* folder).
   - Added Runnables, allowing a user to run multiple files within a project at once.
   - Added Subprojects.
   - Support for opening multiple palettes editing the same type.
   - Runtime code bindings are now done through palettes, which is safer.
   - Palette implementations now support qml code parsers.
   - A type's properties can also be edited by composing palettes.
   - Support for Acts

## 1.5.0

### Support

   - Support for MacOS build and deployment

### Features

   - Support for Palettes and Bindings
   - Palettes for editing colors, numbers and matrices

## 1.4.0

### Compatibility:
   - ```GlobalItem``` is now called ```StaticLoader``` and is available under ```live``` plugin (```import live 1.0```)
   - ```GlobalItemProperty``` is now ```StaticLoaderProperty``` and is available under ```live``` plugin
   - Items that used ```stateId``` are now configured using the ```staticLoad()``` function
(see the documentation on Static Items)
   - ```lcvcontrols``` have been removed, and all items have been distributed under ```lcvcore``` and
```lcvfeatures2d``` plugins
   - ```lcvlib``` api has been removed. Plugins like ```lcvcore```, ```live``` expose their types as libraries, so
the user can now link to them
   - Added a *project* folder for qmake project files developers can now include to extend Livekeys

### Features:
   - Editor configuration file
   - License management
   - Direct runtime property editing
   - Palettes
   - Runtime Binding

## 1.3.0

### Compatibility:
   - All files now require manual import of QtQuick module. (e.g. ```import QtQuick 2.3```)
   - Use ```project.dir()``` instead of ```codeDocument.path``` when refering to the path of the project

### Features:
   - Code Completion support
   - Project management 
   - Project file navigation
   - Script execution and the *live* module
   - Argument forwarding to qml

## 1.2.0

### Features:
   - Added lcvfeatures module.
   - Added build scripts.

## 1.1.0

### Compatibility:

   - Using ```import lcvcore 1.0``` instead of ```import "lcvcore 1.0" ```

### BugFixes:

   - Files are now able to import local directories relative to their location 

## 1.0.0

