# ChangeLog

This file summarises notable changes between Live CV versions.

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
   - Added a *project* folder for qmake project files developers can now include to extend Live CV

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
   - Script execution and the *live* module (```livecv script.qml```)
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

